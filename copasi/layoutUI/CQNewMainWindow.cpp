// Copyright (C) 2010 - 2013 by Pedro Mendes, Virginia Tech Intellectual
// Properties, Inc., University of Heidelberg, and The University
// of Manchester.
// All rights reserved.

// Qt includes
#include <QtCore/QAbstractEventDispatcher>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#include <QtGui/QCloseEvent>
#include <QtGui/QColor>
#include <QtGui/QColorDialog>
#include <QtGui/QComboBox>
#include <QtCore/QFileInfo>
#include <QtGui/QHBoxLayout>
#include <QtGui/QIcon>
#include <QtGui/QImage>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>
#include <QtGui/QMenuBar>
#include <QtGui/QStackedWidget>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>

// local includes
#include "CQGLLayoutPainter.h"
#include "CQGLLayoutViewer.h"
#include "CQNewMainWindow.h"
#include "CQPlayerControlWidget.h"
#include "CQScreenshotOptionsDialog.h"

#include "CQGLLayoutViewer.h"
#include "CQLayoutMainWindow.h"

#include "UI/CQMessageBox.h"
#include "UI/CopasiFileDialog.h"
#include <string.h>

// COPASI includes
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/CopasiDataModel/CCopasiDataModel.h"
#include "copasi/model/CModel.h"
#include "copasi/layout/CLayout.h"
#include "copasi/layout/CListOfLayouts.h"
#include "copasi/layout/CLDefaultStyles.h"

#ifdef ELEMENTARY_MODE_DISPLAY
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/elementaryFluxModes/CEFMTask.h"
#include "copasi/elementaryFluxModes/CEFMProblem.h"
#include "copasi/elementaryFluxModes/CFluxMode.h"
#include "copasi/model/CReaction.h"
#include "copasi/model/CChemEq.h"
#include "copasi/model/CChemEqElement.h"
#include "copasi/model/CMetab.h"
#endif // ELEMENTARY_MODE_DISPLAY

#ifdef COPASI_AUTOLAYOUT
#include "copasi/layout/CCopasiSpringLayout.h"
#include "copasi/layout/CLayoutEngine.h"
#include "copasi/layout/CLayout.h"
#include "copasi/layout/CLGlyphs.h"
#include "copasi/layout/CLBase.h"
#include "copasi/layout/CLCurve.h"
#include "copasi/randomGenerator/CRandom.h"
#include "copasi/report/CCopasiRootContainer.h"
#include "copasi/report/CKeyFactory.h"
#endif // COPASI_AUTOLAYOUT

#include "../UI/icons/photo.xpm"
#include "resourcesUI/CQIconResource.h"
#include "revert_curve.xpm"
#include "film_strip.xpm"
#include "graph.xpm"
#include "load_data.xpm"
#ifdef COPASI_AUTOLAYOUT
#include "layout_start.xpm"
#include "layout_stop.xpm"
#endif // COPASI_AUTOLAYOUT

const char* const CQNewMainWindow::ZOOM_FACTOR_STRINGS[] = {"1%", "2%", "3%", "4%", "5%", "10%", "20%", "25%", "30%", "40%", "50%", "75%", "100%", "150%", "200%", "300%", "400%", "500%", "1000%"};
const double CQNewMainWindow::ZOOM_FACTORS[] = {0.01, 0.02, 0.03, 0.04, 0.05, 0.1, 0.2, 0.25, 0.3, 0.4, 0.5, 0.75, 1.0, 1.5, 2.0, 3.0, 4.0, 5.0, 10.0};

// TODO implement signals that allow enabling and disabling the save and save as actions.

CQNewMainWindow::CQNewMainWindow(CCopasiDataModel* pDatamodel):
  CWindowInterface(),
  mMode(CQNewMainWindow::GRAPH_MODE),
  mpWidgetStack(NULL),
  mpLayoutViewer(NULL),
  mpAnimationWindow(NULL),
  mpDataModel(pDatamodel),
  mpCurrentLayout(NULL),
  mpCurrentRenderInformation(NULL),
  mDocumentChanged(false),
  mCurDir(""),
  mGraphIcon(QPixmap(graph_xpm)),
  mAnimationIcon(QPixmap(film_strip_xpm))
#ifdef ELEMENTARY_MODE_DISPLAY
  , mpFogColorPixmap(new QPixmap(32, 32))
  , mpHighlightColorPixmap(new QPixmap(32, 32))
  , mpHighlightModeAction(NULL)
  , mpChangeColorAction(NULL)
#endif // ELEMENTARY_MODE_DISPLAY
#ifdef COPASI_AUTOLAYOUT
  , mStopLayout(false)
  , mpStopLayoutAction(NULL)
  , mpRandomizeLayout(NULL)
  , mpCalculateDimensions(NULL)
#endif //  COPASI_AUTOLAYOUT
{

#ifndef Darwin
  setWindowIcon(CQIconResource::icon(CQIconResource::copasi));
#endif // not Darwin

  // first we load the default styles if they don't already exist
  if (DEFAULT_STYLES == NULL)
    {
      DEFAULT_STYLES = loadDefaultStyles();
    }

  this->mCurDir = pDatamodel->getReferenceDirectory();
  this->mpWidgetStack = new QStackedWidget(this);
  this->mpLayoutViewer = new CQGLLayoutViewer(this->mpWidgetStack);
  // add the first page
  this->mpWidgetStack->addWidget(this->mpLayoutViewer);
  this->mpAnimationWindow = new CQLayoutMainWindow(this->mpWidgetStack);
  this->mpWidgetStack->addWidget(this->mpAnimationWindow);
  connect(this->mpLayoutViewer, SIGNAL(singleCurveSelected(bool)), this, SLOT(slotSingleCurveSelected(bool)));
  connect(this->mpLayoutViewer->getPainter(), SIGNAL(status_message(const QString&, int)), this, SLOT(setStatusMessage(const QString&, int)));
  createActions();
  createMenus();
  createToolBars();
  createStatusBar();

  this->setCentralWidget(this->mpWidgetStack);
  setUnifiedTitleAndToolBarOnMac(true);
  this->addGlobalRenderInfoItemsToList();
  this->addDefaultRenderInfoItemsToList();
  this->updateLayoutList();
#ifdef ELEMENTARY_MODE_DISPLAY
  // fill the two pixmaps with the current fog and highlight color
  // We have to do that after the call to updateLayoutList because before that call
  // the rnederer does not exist yet.
  const GLfloat* c = this->mpLayoutViewer->getPainter()->getFogColor();
  this->mpFogColorPixmap->fill(QColor((int)(c[0] * 255.0), (int)(c[1] * 255.0), (int)(c[2] * 255.0), (int)(c[3] * 255.0)));
  c = this->mpLayoutViewer->getPainter()->getHighlightColor();
  this->mpHighlightColorPixmap->fill(QColor((int)(c[0] * 255.0), (int)(c[1] * 255.0), (int)(c[2] * 255.0), (int)(c[3] * 255.0)));
  this->mpChangeColorAction->setIcon(QIcon(*this->mpHighlightColorPixmap));
#endif // ELEMENTARY_MODE_DISPLAY
}

QMenu *CQNewMainWindow::getWindowMenu() const
{
  return mpWindowMenu;
}

void CQNewMainWindow::createActions()
{

  mpSwitchModeAct = new QAction(mAnimationIcon, tr("Animation Mode"), this);
  mpSwitchModeAct->setStatusTip(tr("Switch to animation mode."));
  mpSwitchModeAct->setShortcut(Qt::CTRL + Qt::Key_M);
  mpSwitchModeAct->setEnabled(true);
  connect(mpSwitchModeAct, SIGNAL(triggered()), this, SLOT(switchMode()));
  mpRevertCurveAct = new QAction(QPixmap(revert_curve_xpm), tr("Revert curve"), this);
  mpRevertCurveAct->setStatusTip(tr("Reverts the currently selected curve."));
  mpRevertCurveAct->setEnabled(false);
  connect(mpRevertCurveAct, SIGNAL(triggered()), this->mpLayoutViewer, SLOT(slotRevertCurve()));

  mpScreenshotAct = new QAction(QPixmap(photo), tr("Export bitmap..."), this);
  mpScreenshotAct->setStatusTip(tr("Export diagram as bitmap."));
  mpScreenshotAct->setShortcut(Qt::CTRL + Qt::Key_E);
  mpScreenshotAct->setEnabled(true);
  connect(mpScreenshotAct, SIGNAL(triggered()), this, SLOT(slotScreenshot()));

  mpCloseAct = new QAction(tr("Close"), this);
  mpCloseAct->setShortcut(tr("Ctrl+W"));
  mpCloseAct->setStatusTip(tr("Close Diagram"));
  connect(mpCloseAct, SIGNAL(triggered()), this, SLOT(close()));

  mpLoadDataAct = new QAction(QPixmap(load_data_xpm), tr("Update Trajectory Data"), this);
  mpLoadDataAct->setStatusTip(tr("Update time course data"));
  mpLoadDataAct->setEnabled(true);
  connect(this->mpLoadDataAct, SIGNAL(activated()), this->mpAnimationWindow, SLOT(loadData()));

  mpRectangularShape = new QAction("Rectangle", this);
  //mpRectangularShape->setShortcut(Qt::CTRL + Qt::Key_R);
  mpRectangularShape->setStatusTip("Show labels as rectangles");
  connect(mpRectangularShape, SIGNAL(activated()), this->mpAnimationWindow, SLOT(mapLabelsToRectangles()));

  mpCircularShape = new QAction("Circle", this);
  //mpCircularShape->setShortcut(Qt::CTRL + Qt::Key_C);
  mpCircularShape->setStatusTip("Show labels as circles");
  connect(mpCircularShape, SIGNAL(activated()), this->mpAnimationWindow, SLOT(mapLabelsToCircles()));

  mpMimaNodeSizes = new QAction("Set Min/Max Node Sizes", this);
  //mpMimaNodeSizes->setShortcut(Qt::CTRL + Qt::Key_M);
  mpMimaNodeSizes->setToolTip("Change Min/Max for node sizes within animation");
  connect(mpMimaNodeSizes, SIGNAL(activated()), this->mpAnimationWindow, SLOT(changeMinMaxNodeSizes()));

  mpSFontSize = new QAction("Set Font Size", this);
  //mpSFontSize->setShortcut(Qt::CTRL + Qt::Key_F);
  mpSFontSize->setToolTip("Change the font size of the node labels in the graph view");
  connect(mpSFontSize, SIGNAL(activated()), this->mpAnimationWindow, SLOT(changeFontSize()));
#ifdef COPASI_AUTOLAYOUT
  this->mpStopLayoutAction = new QAction(QPixmap(layout_start_xpm), tr("Stop"), this);
  this->mpStopLayoutAction->setEnabled(true);
  this->mpStopLayoutAction->setToolTip("Run Spring Layout Algorithm");
  connect(this->mpStopLayoutAction, SIGNAL(triggered()), this, SLOT(slotRunSpringLayout()));

  mpRandomizeLayout = new QAction(CQIconResource::icon(CQIconResource::roll), tr("Randomize Layout"), this);
  mpRandomizeLayout->setToolTip("Randomize current Layout");
  mpRandomizeLayout->setShortcut(Qt::CTRL + Qt::Key_F5);
  connect(this->mpRandomizeLayout, SIGNAL(triggered()), this, SLOT(slotRunRandomizeLayout()));

  mpCalculateDimensions = new QAction(tr("&Calculate Dimensions"), this);
  mpCalculateDimensions->setToolTip("Calculates Dimensions of this Layout.");
  connect(this->mpCalculateDimensions, SIGNAL(triggered()), this, SLOT(slotCalculateDimensions()));

#endif // COPASI_AUTOLAYOUT
}

void CQNewMainWindow::createMenus()
{
  mpFileMenu = menuBar()->addMenu(tr("&File"));
  mpFileMenu->addAction(mpSwitchModeAct);
  mpFileMenu->addSeparator();
  mpFileMenu->addAction(this->mpScreenshotAct);
  mpFileMenu->addSeparator();
  mpFileMenu->addAction(mpCloseAct);

#ifdef COPASI_AUTOLAYOUT

  mpLayoutMenu = menuBar()->addMenu(tr("&Layout"));
  mpLayoutMenu->addAction(mpStopLayoutAction);
  mpLayoutMenu->addAction(mpRandomizeLayout);
  mpLayoutMenu->addSeparator();
  mpLayoutMenu->addAction(mpCalculateDimensions);
#endif

  // play menu
  mpPlayMenu = menuBar()->addMenu(tr("&Play"));
  mpPlayMenu->setVisible(false);
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getPlayAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getPauseAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getStopAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getForwardAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getBackwardAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getStepForwardAction());
  this->mpPlayMenu->addAction(this->mpAnimationWindow->getControlWidget()->getStepBackwardAction());
  this->mpPlayMenu->addSeparator();
  this->mpLoopItemAction = this->mpPlayMenu->addAction(tr("Loop Animation"));
  this->mpLoopItemAction->setCheckable(true);
  this->mpLoopItemAction->setChecked(false);
  connect(this->mpLoopItemAction, SIGNAL(toggled(bool)) , this->mpAnimationWindow, SLOT(slotLoopActivated(bool)));
  this->mpPlayMenu->addSeparator();
  this->mpPlayMenu->addAction(this->mpLoadDataAct);

  // view menu
  mpViewMenu = menuBar()->addMenu(tr("&View"));
  mpViewMenu->addAction(tr("&Reset View"), this, SLOT(slotResetView()));
  mpViewMenu->addAction(tr("&Fit to Screen"), this, SLOT(slotFitToScreen()));
  this->mpZoomMenu = this->mpViewMenu->addMenu(tr("&Zoom"));
  this->mpZoomActionGroup = new QActionGroup(this);
  QAction* pAction = this->mpZoomActionGroup->addAction("1%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("2%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("3%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("4%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("5%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("10%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("20%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("30%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("40%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("50%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("100%");
  pAction->setCheckable(true);
  pAction->setChecked(true);
  pAction = this->mpZoomActionGroup->addAction("150%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("200%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("300%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("400%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("500%");
  pAction->setCheckable(true);
  pAction = this->mpZoomActionGroup->addAction("1000%");
  pAction->setCheckable(true);
  connect(this->mpZoomActionGroup, SIGNAL(triggered(QAction*)), this, SLOT(slotZoomMenuItemActivated(QAction*)));
  this->mpZoomMenu->addActions(this->mpZoomActionGroup->actions());
#ifdef ELEMENTARY_MODE_DISPLAY
  this->mpViewMenu->addSeparator();
  this->mpHighlightModeAction = this->mpViewMenu->addAction(tr("Highlight"));
  this->mpHighlightModeAction->setCheckable(true);
  this->mpHighlightModeAction->setChecked(true);
  this->mpHighlightModeAction->setToolTip(tr("Toggle whether selected elements are highlighted or unselected items toned down."));
  connect(this->mpHighlightModeAction, SIGNAL(toggled(bool)), this, SLOT(toggleHighlightSlot(bool)));
  this->mpFogDensityAction = this->mpViewMenu->addAction(tr("Fog Density ..."));
  connect(this->mpFogDensityAction, SIGNAL(triggered(bool)), this, SLOT(fogDensitySlot(bool)));
  this->mpChangeColorAction = this->mpViewMenu->addAction(tr("Highlight Color ..."));
  connect(this->mpChangeColorAction, SIGNAL(triggered(bool)), this, SLOT(changeColorSlot(bool)));
  this->mpChangeColorAction->setToolTip(tr("When higlighted, sets the highlight color, or the toned down one otherwise."));
  this->mpElementaryModesMenu = this->mpViewMenu->addMenu("Elementary Modes");
  this->mpElementaryModesMenu->setToolTip(tr("Displays a list of elementary modes when they have been calculated and lets you select one or more that are emphasized in the layout displayed."));
  this->mpElementaryModesMenu->addAction(tr("None"));
  connect(this->mpElementaryModesMenu, SIGNAL(aboutToShow()), this, SLOT(checkForElementaryModesSlot()));
#endif // ELEMENTARY_MODE_DISPLAY

  // options menu
  mpOptionsMenu = menuBar()->addMenu(tr("Options"));
  mpOptionsMenu->setVisible(false);
  QMenu* pM = this->mpOptionsMenu->addMenu(tr("Shape of Label"));
  pM->addAction(this->mpRectangularShape);
  pM->addAction(this->mpCircularShape);
  this->mpOptionsMenu->addAction(this->mpMimaNodeSizes);
  this->mpOptionsMenu->addAction(this->mpSFontSize);

  menuBar()->addSeparator();

  mpWindowMenu = menuBar()->addMenu(tr("&Window"));
}

void CQNewMainWindow::createToolBars()
{
  this->mpFileToolBar = addToolBar(tr("File"));
  this->mpFileToolBar->addAction(this->mpSwitchModeAct);
  this->mpFileToolBar->addAction(this->mpRevertCurveAct);
  this->mpFileToolBar->addAction(this->mpLoadDataAct);
  this->mpLoadDataAct->setVisible(false);
  this->mpFileToolBar->addAction(this->mpScreenshotAct);
#ifdef COPASI_AUTOLAYOUT
  this->mpFileToolBar->addSeparator();
  this->mpFileToolBar->addAction(this->mpStopLayoutAction);
  this->mpFileToolBar->addAction(this->mpRandomizeLayout);
#endif // COPASI_AUTOLAYOUT

  // add a toolbar for the selection widgets
  mpSelectionToolBar = addToolBar(tr("Select"));
  QFrame* pFrame1 = new QFrame;
  QHBoxLayout* pLayout = new QHBoxLayout;
  pLayout->setSpacing(3);
  QLabel* pLabel = new QLabel(tr("Layout:"));
  pLayout->addWidget(pLabel);
  this->mpLayoutDropdown = new QComboBox;
  this->mpLayoutDropdown->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
  pFrame1->setLayout(pLayout);
  pLayout->addWidget(this->mpLayoutDropdown);

  QFrame* pFrame2 = new QFrame;
  pLayout = new QHBoxLayout;
  pLayout->setSpacing(3);
  pFrame2->setLayout(pLayout);
  this->mpRenderLabel = new QLabel(tr("Render Information:"));
  pLayout->addWidget(this->mpRenderLabel);
  this->mpRenderDropdown = new QComboBox;
  this->mpRenderDropdown->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);
  pLayout->addWidget(this->mpRenderDropdown);

  QFrame* pFrame3 = new QFrame;
  pLayout = new QHBoxLayout;
  pLayout->setSpacing(3);
  pFrame3->setLayout(pLayout);
  pLabel = new QLabel(tr("Zoom Factor:"));
  pLayout->addWidget(pLabel);
  this->mpZoomDropdown = new QComboBox;
  pLayout->addWidget(this->mpZoomDropdown);
  // fill the zoom factor box
  unsigned int i, iMax = sizeof(CQNewMainWindow::ZOOM_FACTOR_STRINGS) / sizeof(char*);
  int defaultIndex = -1;

  for (i = 0; i < iMax; ++i)
    {
      this->mpZoomDropdown->addItem(QString(CQNewMainWindow::ZOOM_FACTOR_STRINGS[i]));

      if (std::string(CQNewMainWindow::ZOOM_FACTOR_STRINGS[i]) == std::string("100%"))
        {
          defaultIndex = i;
        }
    }

  this->mpZoomDropdown->setSizeAdjustPolicy(QComboBox::AdjustToContentsOnFirstShow);

  // set 100% as the default zoom factor
  assert(defaultIndex != -1);
  this->mpZoomDropdown->setCurrentIndex(defaultIndex);
  this->mpSelectionToolBar->addWidget(pFrame1);
  this->mpSelectionToolBar->addWidget(pFrame2);
  this->mpSelectionToolBar->addWidget(pFrame3);
  // connect the combobox signals
  connect(this->mpLayoutDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLayoutChanged(int)));
  connect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
  connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
}

void CQNewMainWindow::createStatusBar()
{
  statusBar()->showMessage(tr("Ready"));
}
void CQNewMainWindow::setMode(DISPLAY_MODE mode)
{
  // be sure to disconnect screenshot action
  if (mMode == GRAPH_MODE)
    disconnect(mpScreenshotAct, SIGNAL(triggered()), this, SLOT(slotScreenshot()));
  else
    disconnect(this->mpScreenshotAct, SIGNAL(triggered()), this->mpAnimationWindow, SLOT(saveImage()));

  // need to invert to have the toggle work
  mMode = mode == GRAPH_MODE ? ANIMATION_MODE : GRAPH_MODE;
  switchMode();
}
void CQNewMainWindow::updateRenderer()
{
  // pass the datamodel, the layout and the render info to the
  // LayoutViewer
  this->resetView();
  this->mpLayoutViewer->update(this->mpDataModel, this->mpCurrentLayout, this->mpCurrentRenderInformation, QString(this->mCurDir.c_str()));
  this->mpAnimationWindow->setLayout(this->mpCurrentLayout);

  if (this->mpCurrentRenderInformation == getDefaultStyle(0))
    {
      this->mpLayoutViewer->change_style(this->mpCurrentRenderInformation, true);
    }
}

void CQNewMainWindow::resetView()
{
  unsigned int i, iMax = sizeof(CQNewMainWindow::ZOOM_FACTOR_STRINGS) / sizeof(char*);
  int defaultIndex = -1;

  for (i = 0; i < iMax; ++i)
    {
      if (std::string(CQNewMainWindow::ZOOM_FACTOR_STRINGS[i]) == std::string("100%"))
        {
          defaultIndex = i;
        }
    }

  // set 100% as the default zoom factor
  assert(defaultIndex != -1);
  disconnect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
  this->mpZoomDropdown->setCurrentIndex(defaultIndex);
  connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
}

void CQNewMainWindow::slotResetView()
{
  this->resetView();
  // if we are not in animation mode, we have to repaint
  this->mpLayoutViewer->setZoomFactor(1.0);
  this->mpAnimationWindow->slotResetView();
}

/**
 * This slot is called when the "Fit To Screen" menu item
 * is activated.
 */
void CQNewMainWindow::slotFitToScreen()
{
  double zoom = 1.0;

  if (this->mMode == CQNewMainWindow::ANIMATION_MODE)
    {
      zoom = this->mpAnimationWindow->slotFitToScreen();
      // set the zoom factor on the other GL window
      this->mpLayoutViewer->setZoomFactor(zoom);
    }
  else
    {
      zoom = this->mpLayoutViewer->fitToScreen();
      // set the zoom factor on the other GL window
      this->mpAnimationWindow->setZoomFactor(zoom);
    }

  // uncheck the zoom entry in the menu
  QList<QAction*> actions = this->mpZoomActionGroup->actions();
  QList<QAction*>::iterator it = actions.begin(), endit = actions.end();

  while (it != endit)
    {
      if ((*it)->isChecked())
        {
          (*it)->setChecked(false);
          // only one can be checked
          break;
        }

      ++it;
    }

  // add a new entry for the current zoom factor to the zoom dropdown list
  disconnect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
  // add a new entry for the zoom factor
  QString s = QString("%1").arg(zoom * 100);
  s.append("%");
  const size_t n = sizeof(CQNewMainWindow::ZOOM_FACTOR_STRINGS) / sizeof(char*);

  if ((size_t)this->mpZoomDropdown->count() > n)
    {
      this->mpZoomDropdown->setItemText(0, s);
    }
  else
    {
      this->mpZoomDropdown->insertItem(0, s);
    }

  this->mpZoomDropdown->setCurrentIndex(0);
  connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
}

void CQNewMainWindow::slotLayoutChanged(int index)
{
  //std::cout << "new layout " << index << std::endl;
  CLayout* pTmpLayout = (*this->mpDataModel->getListOfLayouts())[index];

  if (pTmpLayout != this->mpCurrentLayout)
    {
      this->mpCurrentLayout = pTmpLayout;
      // update the corresponding render information list
      this->updateRenderInformationList();
      this->updateRenderer();
    }
}

void CQNewMainWindow::slotRenderInfoChanged(int index)
{
  // check if a local or a global render information has been selected
  CLRenderInformationBase* pRenderInfo = NULL;

  // the local render information is first in the list
  if (index == -1)
    {
      //std::cout << "index is -1" << std::endl;
      return;
    }

  size_t Index = (size_t) index;

  size_t numLocalRenderInfo = this->mpCurrentLayout->getListOfLocalRenderInformationObjects().size();
  size_t numFileRenderInfo = numLocalRenderInfo + this->mpDataModel->getListOfLayouts()->getListOfGlobalRenderInformationObjects().size();

  if (Index >= numLocalRenderInfo)
    {
      // it is a global render information or a default render info
      if (Index >= numFileRenderInfo)
        {
          // it must be a default render information
          pRenderInfo = getDefaultStyle(Index - numFileRenderInfo);

          if (pRenderInfo != this->mpCurrentRenderInformation)
            {
              this->mpCurrentRenderInformation = pRenderInfo;
              this->change_style(true);
            }
        }
      else
        {
          pRenderInfo = this->mpDataModel->getListOfLayouts()->getRenderInformation(Index - numLocalRenderInfo);

          if (pRenderInfo != this->mpCurrentRenderInformation)
            {
              this->mpCurrentRenderInformation = pRenderInfo;
              this->change_style();
            }
        }
    }
  else
    {
      // it is a local render information
      pRenderInfo = this->mpCurrentLayout->getRenderInformation(Index);

      if (pRenderInfo != this->mpCurrentRenderInformation)
        {
          this->mpCurrentRenderInformation = pRenderInfo;
          this->change_style();
        }
    }
}

void CQNewMainWindow::updateRenderInformationList()
{
  // disconnect the slot
  disconnect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
  // remove the local render information items
  size_t num = this->mpDataModel->getListOfLayouts()->getListOfGlobalRenderInformationObjects().size() + getNumDefaultStyles();

  while ((size_t) this->mpRenderDropdown->count() > num)
    {
      this->mpRenderDropdown->removeItem(0);
    }

  // add the new local render information items
  if (this->mpCurrentLayout)
    {
      num = this->mpCurrentLayout->getListOfLocalRenderInformationObjects().size();
      size_t i;
      CLRenderInformationBase* pTmpRenderInfo = NULL;

      for (i = num; i > 0; --i)
        {
          pTmpRenderInfo = this->mpCurrentLayout->getRenderInformation(i - 1);
          std::string text = pTmpRenderInfo->getKey();

          if (!pTmpRenderInfo->getName().empty())
            {
              text = pTmpRenderInfo->getName();
              //text += " (";
              //text += pTmpRenderInfo->getName();
              //text += ")";
            }

          this->mpRenderDropdown->insertItem(0, QString(text.c_str()));
        }
    }

  // if the current render information is a global render information, keep
  // the selection
  // otherwise select the first render information if there is one
  if (dynamic_cast<const CLGlobalRenderInformation*>(this->mpCurrentRenderInformation))
    {
      // find the correct index
      size_t i, iMax = this->mpDataModel->getListOfLayouts()->getListOfGlobalRenderInformationObjects().size();

      for (i = 0; i < iMax; ++i)
        {
          if (this->mpCurrentRenderInformation == this->mpDataModel->getListOfLayouts()->getRenderInformation(i))
            {
              break;
            }
        }

      if (i == iMax)
        {
          // it must be a default style
          num += i;
          iMax = getNumDefaultStyles();

          for (i = 0; i < iMax; ++i)
            {
              if (this->mpCurrentRenderInformation == getDefaultStyle(i))
                {
                  break;
                }
            }

          assert(i != iMax);
          this->mpRenderDropdown->setCurrentIndex((int)(num + i));
        }
      else
        {
          this->mpRenderDropdown->setCurrentIndex((int)(num + i));
        }
    }
  else
    {
      if (this->mpRenderDropdown->count() > 0)
        {
          if (num > 0)
            {
              // take the first local render information
              if (this->mpCurrentLayout != NULL)
                {
                  this->mpCurrentRenderInformation = this->mpCurrentLayout->getRenderInformation(0);
                }
              else
                {
                  this->mpCurrentRenderInformation = NULL;
                }

              this->mpRenderDropdown->setCurrentIndex(0);
            }
          else if (this->mpDataModel->getListOfLayouts()->getListOfGlobalRenderInformationObjects().size() > 0)
            {
              // take the first global render information
              this->mpCurrentRenderInformation = this->mpDataModel->getListOfLayouts()->getRenderInformation(0);
              this->mpRenderDropdown->setCurrentIndex((int) num);
            }
          else if (getNumDefaultStyles() > 0)
            {
              this->mpCurrentRenderInformation = getDefaultStyle(0);
            }
          else
            {
              this->mpCurrentRenderInformation = NULL;
            }
        }
      else
        {
          this->mpCurrentRenderInformation = NULL;
        }
    }

  // reconnect the slot
  connect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
}

void CQNewMainWindow::addGlobalRenderInfoItemsToList()
{
  // disconnect the slot
  disconnect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
  this->mpRenderDropdown->clear();
  size_t i, iMax = this->mpDataModel->getListOfLayouts()->getListOfGlobalRenderInformationObjects().size();
  CLRenderInformationBase* pTmpRenderInfo = NULL;

  for (i = 0; i < iMax; ++i)
    {
      pTmpRenderInfo = this->mpDataModel->getListOfLayouts()->getRenderInformation(i);
      std::string text = pTmpRenderInfo->getKey();

      if (!pTmpRenderInfo->getName().empty())
        {
          text = pTmpRenderInfo->getName();
          //text += " (";
          //text += pTmpRenderInfo->getName();
          //text += ")";
        }

      this->mpRenderDropdown->addItem(QString(text.c_str()));
    }

  // reconnect the slot
  connect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
}

void CQNewMainWindow::addDefaultRenderInfoItemsToList()
{
  // disconnect the slot
  disconnect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
  size_t i, iMax = getNumDefaultStyles();
  CLRenderInformationBase* pTmpRenderInfo = NULL;

  for (i = 0; i < iMax; ++i)
    {
      pTmpRenderInfo = getDefaultStyle(i);
      std::string text = pTmpRenderInfo->getKey();

      if (!pTmpRenderInfo->getName().empty())
        {
          text = pTmpRenderInfo->getName();
          //text += " (";
          //text += pTmpRenderInfo->getName();
          //text += ")";
        }

      this->mpRenderDropdown->addItem(QString(text.c_str()));
    }

  // reconnect the slot
  connect(this->mpRenderDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotRenderInfoChanged(int)));
}

void CQNewMainWindow::slotZoomChanged(int index)
{
  // check if the number of zoom factors in the combobox is greater than
  // the numbers of items in the zoom combo box
  //
  // if so, delete the first entry from the list and reduce the index by 1 because the first entry must be
  // the item that has been added by fitToScreen.
  const size_t n = sizeof(CQNewMainWindow::ZOOM_FACTOR_STRINGS) / sizeof(char*);

  if ((size_t)this->mpZoomDropdown->count() > n && index != 0)
    {
      --index;
      disconnect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
      this->mpZoomDropdown->removeItem(0);
      this->mpZoomDropdown->setCurrentIndex(index);
      connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
    }

  this->mpLayoutViewer->setZoomFactor(CQNewMainWindow::ZOOM_FACTORS[index]);
  // also change the zoom factor for the animation window
  this->mpAnimationWindow->setZoomFactor(this->mpZoomDropdown->currentText());

  // also set the zoom factor in the menu
  if ((int) index < this->mpZoomActionGroup->actions().size())
    {
      this->mpZoomActionGroup->actions().at(index)->setChecked(true);
    }
}

void CQNewMainWindow::slotZoomMenuItemActivated(QAction* pAction)
{
  int index = this->mpZoomActionGroup->actions().indexOf(pAction);
  // check if the number of zoom factors in the combobox is greater than
  // the number of items in the combobox
  //
  // if so, delete the first entry from the list because this
  // item that has been added by fitToScreen.
  const size_t n = sizeof(CQNewMainWindow::ZOOM_FACTOR_STRINGS) / sizeof(char*);

  if ((size_t)this->mpZoomDropdown->count() > n)
    {
      disconnect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
      this->mpZoomDropdown->removeItem(0);
      connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));
    }

  this->mpLayoutViewer->setZoomFactor(CQNewMainWindow::ZOOM_FACTORS[index]);
  // also set the zoom factor in the combobox
  disconnect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));

  if (index < this->mpZoomDropdown->count())
    {
      this->mpZoomDropdown->setCurrentIndex(index);
    }

  connect(this->mpZoomDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotZoomChanged(int)));

  // also change the zoom factor for the animation window
  this->mpAnimationWindow->setZoomFactor(this->mpZoomDropdown->currentText());
}

void CQNewMainWindow::change_style(bool defaultStyle)
{
  this->mpLayoutViewer->change_style(this->mpCurrentRenderInformation, defaultStyle);
}

/**
 * This slot is called when something in the document changes.
 */
void CQNewMainWindow::slotSingleCurveSelected(bool selected)
{
  // check if a single curve is selected
  if (selected)
    {
      this->mpRevertCurveAct->setEnabled(true);
    }
  else
    {
      this->mpRevertCurveAct->setEnabled(false);
    }
}

/**
 * This slot initiates the export
 * of the layout as a bitmap.
 * The user gets to choose which part of the
 * layout is to be exported and how large the
 * resulting bitmap is supposed to be.
 */
void CQNewMainWindow::slotScreenshot()
{
  CQGLLayoutPainter* pPainter = this->mpLayoutViewer->getPainter();

  if (pPainter == NULL)
    return;

  double layoutX = pPainter->minX();
  double layoutY = pPainter->minY();
  double layoutWidth = pPainter->maxX() - layoutX;
  double layoutHeight = pPainter->maxY() - layoutY;
  double x = pPainter->getCurrentPositionX();
  double y = pPainter->getCurrentPositionY();
  double width = pPainter->getCurrentWidth();
  double height = pPainter->getCurrentHeight();
#ifndef USE_SCREENSHOT_OPTIONS

  QString fileName = CopasiFileDialog::getSaveFileName(this, QString("Export to"), "", QString("PNG (*.png);;All files (*)"));

  if (!fileName.isEmpty())
    export_bitmap(fileName, 2.0);

#else
  CQScreenshotOptionsDialog* pDialog = new CQScreenshotOptionsDialog(layoutX, layoutY, layoutWidth, layoutHeight,
      x, y, width, height, pPainter->width() , pPainter->height(), -1, this);

  if (pDialog->exec() == QDialog::Accepted)
    {

      // ask for the filename
      QString fileName = CopasiFileDialog::getSaveFileName(this, QString("Export to"), "", QString("PNG (*.png);;All files (*)"));

      if (!fileName.isEmpty())
        {
          export_bitmap(fileName, pDialog->getX(), pDialog->getY(), pDialog->getWidth(), pDialog->getHeight(), pDialog->getImageWidth(), pDialog->getImageHeight(), pDialog->isSetDrawSelectionDecoration());
        }
    }

  delete pDialog;
#endif
}

void CQNewMainWindow::export_bitmap(const QString& filename, double scale /*= 4.0*/)
{
  CQGLLayoutPainter* pPainter = this->mpLayoutViewer->getPainter();

  if (pPainter == NULL)
    return;

  double layoutX = pPainter->minX();
  double layoutY = pPainter->minY();
  double layoutWidth = pPainter->maxX() - layoutX;
  double layoutHeight = pPainter->maxY() - layoutY;

  export_bitmap(filename, layoutX, layoutY, layoutWidth, layoutHeight, layoutWidth * scale, layoutHeight * scale, false);
}

/**
 * Exports a bitmap of the given size to
 * the file with the given name.
 */
void CQNewMainWindow::export_bitmap(const QString& filename, double x, double y, double width, double height, unsigned int imageWidth, unsigned int imageHeight, bool drawSelection)
{
  // check if the size of the final image is ok.
  double size = imageWidth * imageHeight * 4;

  // TODO: not sure we really need to restrict this, if the user *needs* a higher resolution image
  // TODO: what speeks against it? (Though in praxis resolutions above 4320p are unlikely)
  // I don't think we should write images that are larger than 500MB
  if (size > 5e8)
    {
      // give an error message that the image is to large
      CQMessageBox::critical(this, tr("Image too large"),
                             tr("Sorry, refusing to create images that are larger than 500MB."));
    }

  //
  // before we even start, we check if the file can be written to at all
  // if not, we don't have to do anything
  QFileInfo info(filename);

  if (info.exists())
    {
      if (!info.isFile())
        {
          // create an error message and cancel saving
          CQMessageBox::critical(this, tr("Not a file"),
                                 tr("Path exists, but it is not a file."));
          return;
        }
      else
        {
          if (!info.isWritable())
            {
              // create an error message and cancel saving
              CQMessageBox::critical(this, tr("Not writable"),
                                     tr("Can't write to file."));
              return;
            }
        }
    }

  // now we try to create the image
  CQGLLayoutPainter* pPainter = this->mpLayoutViewer->getPainter();

  if (pPainter == NULL)
    return;

  GLubyte* pImageData = pPainter->export_bitmap(x, y, width, height, imageWidth, imageHeight, drawSelection);

  if (pImageData == NULL)
    return;

  // create QImage for buffer
  QImage* pImage = new QImage(pImageData, imageWidth, imageHeight, 4 * imageWidth, QImage::Format_ARGB32);
  bool result = pImage->save(filename, "PNG");

  if (result == false)
    {
      CQMessageBox::critical(this, tr("Save error"),
                             tr("An error occured while saving the file.\nThe file might be invalid."));
    }

  delete pImage;
  delete[] pImageData;
}

void CQNewMainWindow::updateLayoutList()
{
  if (this->mpDataModel == NULL) return;

  disconnect(this->mpLayoutDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLayoutChanged(int)));
  this->mpLayoutDropdown->clear();
  const CLayout* pTmpLayout = NULL;
  size_t i, iMax = this->mpDataModel->getListOfLayouts()->size();

  for (i = 0; i < iMax; ++i)
    {
      pTmpLayout = (*this->mpDataModel->getListOfLayouts())[i];
      std::string text = pTmpLayout->getKey();

      if (!pTmpLayout->getObjectName().empty())
        {
          text = pTmpLayout->getObjectName();
          //text += "(";
          //text += pTmpLayout->getObjectName();
          //text += ")";
        }

      this->mpLayoutDropdown->addItem(text.c_str());
    }

  if (iMax > 0)
    {
      this->mpCurrentLayout = (*this->mpDataModel->getListOfLayouts())[0];
    }

  this->updateRenderInformationList();

  if (this->mpCurrentLayout && this->mpCurrentRenderInformation)
    {
      this->updateRenderer();
    }

  connect(this->mpLayoutDropdown, SIGNAL(currentIndexChanged(int)), this, SLOT(slotLayoutChanged(int)));
}

/**
 * switches the display mode between animation mode and
 * graph mode.
 */
void CQNewMainWindow::switchMode()
{
  switch (this->mMode)
    {
      case CQNewMainWindow::GRAPH_MODE:
        disconnect(mpScreenshotAct, SIGNAL(triggered()), this, SLOT(slotScreenshot()));
        connect(this->mpScreenshotAct, SIGNAL(triggered()), this->mpAnimationWindow, SLOT(saveImage()));
        this->mpSwitchModeAct->setIcon(mGraphIcon);
        this->mpSwitchModeAct->setStatusTip(tr("Switch to graph mode."));
        this->mpSwitchModeAct->setText(tr("Graph Mode"));
        this->mMode = CQNewMainWindow::ANIMATION_MODE;
        this->setAnimationToolbar();
        this->setAnimationMenu();
        this->mpWidgetStack->setCurrentIndex(1);
#ifdef ELEMENTARY_MODE_DISPLAY
        this->mpElementaryModesMenu->setEnabled(false);
        this->mpHighlightModeAction->setEnabled(false);
        this->mpChangeColorAction->setEnabled(false);
#endif // ELEMENTARY_MODE_DISPLAY

#ifdef COPASI_AUTOLAYOUT
        updateLayoutList();
        // reset current displayed layout, so we can get the updates displayed
        this->mpAnimationWindow->setLayout(NULL);
        // update with the new layout
        this->mpAnimationWindow->setLayout(mpCurrentLayout);
        // and fit it too screen
        slotFitToScreen();
#endif
        break;

      case CQNewMainWindow::ANIMATION_MODE:
        disconnect(this->mpScreenshotAct, SIGNAL(triggered()), this->mpAnimationWindow, SLOT(saveImage()));
        connect(mpScreenshotAct, SIGNAL(triggered()), this, SLOT(slotScreenshot()));
        this->mpSwitchModeAct->setIcon(mAnimationIcon);
        this->mpSwitchModeAct->setStatusTip(tr("Switch to animation mode."));
        this->mpSwitchModeAct->setText(tr("Animation Mode"));
        this->mMode = CQNewMainWindow::GRAPH_MODE;
        this->setGraphToolbar();
        this->setGraphMenu();
        this->mpWidgetStack->setCurrentIndex(0);
#ifdef ELEMENTARY_MODE_DISPLAY
        this->mpElementaryModesMenu->setEnabled(true);
        this->mpHighlightModeAction->setEnabled(true);
        this->mpChangeColorAction->setEnabled(true);
#endif // ELEMENTARY_MODE_DISPLAY
        break;
    }
}

void CQNewMainWindow::setAnimationToolbar()
{
  // remove revert curve action
  this->mpRevertCurveAct->setVisible(false);
  // add load data action
  this->mpLoadDataAct->setVisible(true);
  // hide the render information box
  this->mpRenderLabel->hide();
  this->mpRevertCurveAct->setVisible(false);
  this->mpRenderDropdown->hide();
}

void CQNewMainWindow::setGraphToolbar()
{
  // remove load data action
  this->mpLoadDataAct->setVisible(false);
  // add revert curve action
  this->mpRevertCurveAct->setVisible(true);
  // show the render information box
  this->mpRenderLabel->show();
  this->mpRevertCurveAct->setVisible(false);
  this->mpRenderDropdown->show();
}

void CQNewMainWindow::setAnimationMenu()
{
  this->mpPlayMenu->menuAction()->setVisible(true);
#if COPASI_AUTOLAYOUT
  this->mpLayoutMenu->menuAction()->setVisible(false);
#endif
  this->mpOptionsMenu->menuAction()->setVisible(true);
}

void CQNewMainWindow::setGraphMenu()
{
  this->mpPlayMenu->menuAction()->setVisible(false);
#if COPASI_AUTOLAYOUT
  this->mpLayoutMenu->menuAction()->setVisible(true);
#endif
  this->mpOptionsMenu->menuAction()->setVisible(false);
}

void CQNewMainWindow::setStatusMessage(const QString& message, int timeout)
{
  this->statusBar()->showMessage(message, timeout);
}

#ifdef ELEMENTARY_MODE_DISPLAY
/**
 * Checks for calculated elementary modes.
 */
void CQNewMainWindow::checkForElementaryModesSlot()
{
  bool fluxModesChanged = false;

  if (this->mpDataModel != NULL)
    {
      const CCopasiVectorN< CCopasiTask >* pTaskList = this->mpDataModel->getTaskList();
      assert(pTaskList != NULL);

      if (pTaskList != NULL)
        {
          // get the metabolic control analysis task object
          const CEFMTask* pTask = dynamic_cast<const CEFMTask*>((*pTaskList)["Elementary Flux Modes"]);

          if (pTask != NULL)
            {
              const CEFMProblem* pProblem = dynamic_cast<const CEFMProblem*>(pTask->getProblem());

              if (pProblem != NULL && !pProblem->getFluxModes().empty())
                {
                  const std::vector< CFluxMode >& fluxModes = pProblem->getFluxModes();
                  size_t iMax = fluxModes.size();

                  if (this->mFluxModes.size() != iMax)
                    {
                      fluxModesChanged = true;
                    }
                  else
                    {
                      unsigned int i = 0;

                      while (i < iMax && fluxModesChanged == false)
                        {
                          if (&(fluxModes[i]) != this->mFluxModes[i])
                            {
                              fluxModesChanged = true;
                            }

                          ++i;
                        }
                    }

                  if (fluxModesChanged)
                    {
                      this->mpElementaryModesMenu->clear();
                      this->mFluxModes.clear();

                      if (fluxModes.empty())
                        {
                          this->mpElementaryModesMenu->addAction("None");
                          disconnect(this->mpElementaryModesMenu, SIGNAL(triggered(QAction*)), this, SLOT(elementaryModeTriggeredSlot(QAction*)));
                        }
                      else
                        {
                          unsigned int i = 0;
                          const CFluxMode* pFluxMode = NULL;

                          while (i < iMax)
                            {
                              pFluxMode = &(fluxModes[i]);
                              this->mFluxModes.push_back(pFluxMode);
                              std::string desc = pTask->getFluxModeDescription(*pFluxMode);
                              QAction* pAction = this->mpElementaryModesMenu->addAction(QString(desc.c_str()));
                              pAction->setCheckable(true);
                              ++i;
                            }

                          connect(this->mpElementaryModesMenu, SIGNAL(triggered(QAction*)), this, SLOT(elementaryModeTriggeredSlot(QAction*)));
                        }
                    }
                }
            }
        }
    }
}

/**
 * Checks which elementary mode has been toggled and updates the
 * highlighted objects list.
 */
void CQNewMainWindow::elementaryModeTriggeredSlot(QAction* pAction)
{
  CQGLLayoutPainter* pPainter = this->mpLayoutViewer->getPainter();

  if (pPainter == NULL) return;

  // determine the index of the flux mode
  unsigned int index = this->mpElementaryModesMenu->actions().indexOf(pAction);
  // if the action is checked, we need to add all the reactions of the flux mode
  // to the highlighted objects
  unsigned int mask = CQNewMainWindow::REACTION_GLYPH | CQNewMainWindow::ROLE_SUBSTRATE | CQNewMainWindow::ROLE_PRODUCT | CQNewMainWindow::ROLE_SIDESUBSTRATE | CQNewMainWindow::ROLE_SIDEPRODUCT | CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS;

  if (pAction->isChecked())
    {
      const CFluxMode* pFlux = this->mFluxModes[index];
      CFluxMode::const_iterator it = pFlux->begin(), endit = pFlux->end();
      const CReaction* pReaction = NULL;
      assert(this->mpDataModel != NULL && this->mpDataModel->getModel() != NULL);
      const CModel* pModel = this->mpDataModel->getModel();
      const CEFMProblem* pProblem = NULL;
      const CCopasiVectorN< CCopasiTask >* pTaskList = this->mpDataModel->getTaskList();
      assert(pTaskList != NULL);

      if (pTaskList != NULL)
        {
          // get the metabolic control analysis task object
          const CEFMTask* pTask = dynamic_cast<const CEFMTask*>((*pTaskList)["Elementary Flux Modes"]);

          if (pTask != NULL)
            {
              pProblem = dynamic_cast<const CEFMProblem*>(pTask->getProblem());
            }
        }

      while (it != endit && pProblem != NULL)
        {
          assert(pModel->getReactions().size() > it->first);

          // the index is the index of the reordered fluxes from the problem, so in order
          // to find the correct reaction, we need to get the reaction from the problem
          if (pProblem != NULL)
            {
              pReaction = pProblem->getReorderedReactions()[it->first];
              assert(pReaction != NULL);
              this->selectReaction(pReaction, mask, pPainter->getHighlightedObjects());
            }

          ++it;
        }
    }
  // if the node is unchecked, we need to remove all the reactions from the
  // highlighted objects, but only those that are not part of another selected flux mode
  else
    {
      // we first need to find all objects that belong to other flux modes
      const QList<QAction*>& actions = this->mpElementaryModesMenu->actions();
      QList<QAction*>::const_iterator ait = actions.begin(), aendit = actions.end();
      unsigned int i;
      const CFluxMode* pFlux = NULL;

      // we remove all highlighted objects
      std::set<const CLGraphicalObject*>& s = pPainter->getHighlightedObjects();
      s.clear();
      this->mHighlightedReactions.clear();

      // and readd the ones that remain
      while (ait != aendit)
        {
          if ((*ait)->isChecked())
            {
              i = actions.indexOf(*ait);
              assert(i < this->mFluxModes.size());
              pFlux = this->mFluxModes[i];
              CFluxMode::const_iterator it = pFlux->begin(), endit = pFlux->end();
              const CReaction* pReaction = NULL;
              assert(this->mpDataModel != NULL && this->mpDataModel->getModel() != NULL);
              const CModel* pModel = this->mpDataModel->getModel();

              while (it != endit)
                {
                  assert(pModel->getReactions().size() > it->first);
                  pReaction = this->mpDataModel->getModel()->getReactions()[it->first];
                  assert(pReaction != NULL);
                  this->selectReaction(pReaction, mask, s);
                  ++it;
                }
            }

          ++ait;
        }
    }

  // redraw the GL window
  if (this->mMode == GRAPH_MODE)
    {
      this->mpLayoutViewer->getPainter()->update();
    }
}

/**
 * Selects the given reaction object by selecting all
 * corresponding CLReactionGlyph objects in the current layout.
 * The mask determines which parts of the reaction are selected.
 * With this, it can be specified that only the reaction glyph itself
 * or the reaction glyph plus several of the associated metab reference glyphs
 * are selected.
 * The graphical objects selected by this method are inserted into the
 * set given as the third element.
 */
void CQNewMainWindow::selectReaction(const CReaction* pReaction, unsigned int selectionMask, std::set<const CLGraphicalObject*>& s)
{
  // we need a reaction and something to select in that
  // reaction
  if (pReaction != NULL && selectionMask != 0 && this->mpCurrentLayout != NULL)
    {
      CQNewMainWindow::REACTION_SELECTION_ITEM item;
      item.mReactionKey = pReaction->getKey();
      item.mSelectionMask = selectionMask;
      this->mHighlightedReactions.insert(item);
      // go through the metabolite glyphs and select
      // all metabolite glyphs that are associated
      const CCopasiVector<CLReactionGlyph>& v = this->mpCurrentLayout->getListOfReactionGlyphs();
      CCopasiVector<CLReactionGlyph>::const_iterator it = v.begin(), endit = v.end();

      while (it != endit)
        {
          if ((*it)->getModelObject() == pReaction)
            {
              if (selectionMask & CQNewMainWindow::REACTION_GLYPH)
                {
                  s.insert((*it));
                }

              const CCopasiVector<CLMetabReferenceGlyph>& mrgv = (*it)->getListOfMetabReferenceGlyphs();

              CCopasiVector<CLMetabReferenceGlyph>::const_iterator it2 = mrgv.begin(), endit2 = mrgv.end();

              std::string key;

              const CCopasiObject* pObject = NULL;

              while (it2 != endit2)
                {
                  // check the role
                  switch ((*it2)->getRole())
                    {
                      case CLMetabReferenceGlyph::UNDEFINED:

                        if (selectionMask & CQNewMainWindow::ROLE_UNSPECIFIED)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::SUBSTRATE:

                        if (selectionMask & CQNewMainWindow::ROLE_SUBSTRATE)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::PRODUCT:

                        if (selectionMask & CQNewMainWindow::ROLE_PRODUCT)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::SIDESUBSTRATE:

                        if (selectionMask & CQNewMainWindow::ROLE_SIDESUBSTRATE)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::SIDEPRODUCT:

                        if (selectionMask & CQNewMainWindow::ROLE_SIDEPRODUCT)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::MODIFIER:

                        if (selectionMask & CQNewMainWindow::ROLE_MODIFIER)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::ACTIVATOR:

                        if (selectionMask & CQNewMainWindow::ROLE_ACTIVATOR)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;

                      case CLMetabReferenceGlyph::INHIBITOR:

                        if (selectionMask & CQNewMainWindow::ROLE_INHIBITOR)
                          {
                            s.insert(*it2);
                            key = (*it2)->getMetabGlyphKey();
                            pObject = CCopasiRootContainer::getKeyFactory()->get(key);

                            if (pObject != NULL && dynamic_cast<const CLMetabGlyph*>(pObject) != NULL && (selectionMask & CQNewMainWindow::ASSOCIATED_SPECIES_GLYPHS))
                              {
                                s.insert(static_cast<const CLMetabGlyph*>(pObject));
                              }
                          }

                        break;
                    }

                  ++it2;
                }
            }

          ++it;
        }
    }
}

/**
 * Selected the given metabolite object by selecting all
 * corresponding CLMetabGlyph objects in the current layout.
 * The graphical objects selected by this method are inserted into the
 * set given as the third element.
 */
void CQNewMainWindow::selectMetabolite(const CMetab* pMetab, std::set<const CLGraphicalObject*>& s)
{
  if (pMetab != NULL && this->mpCurrentLayout != NULL)
    {
      this->mHighlightedMetabolites.insert(pMetab->getKey());
      // go through the metabolite glyphs and select
      // all metabolite glyphs that are associated
      const CCopasiVector<CLMetabGlyph>& v = this->mpCurrentLayout->getListOfMetaboliteGlyphs();
      CCopasiVector<CLMetabGlyph>::const_iterator it = v.begin(), endit = v.end();

      while (it != endit)
        {
          if ((*it)->getModelObject() == pMetab)
            {
              s.insert((*it));
            }

          ++it;
        }
    }
}

/**
 * Is called when the menu entry for toggling highlighting
 * of elementary modes is toggled.
 */
void CQNewMainWindow::toggleHighlightSlot(bool checked)
{
  this->mpLayoutViewer->getPainter()->setHighlightFlag(checked);

  // update the icon and the text
  if (checked)
    {
      this->mpChangeColorAction->setText(tr("highlight color ..."));
      this->mpChangeColorAction->setIcon(QIcon(*this->mpHighlightColorPixmap));
    }
  else
    {
      this->mpChangeColorAction->setText(tr("fog color ..."));
      this->mpChangeColorAction->setIcon(QIcon(*this->mpFogColorPixmap));
    }

  // we need to redraw the gl window
  if (this->mMode == GRAPH_MODE)
    {
      this->mpLayoutViewer->getPainter()->update();
    }
}

/**
 * Lets the user change the percentage of fog
 * that is added to the color.
 */
void CQNewMainWindow::fogDensitySlot(bool)
{
  // show a dialog with a spin box that goes from 0 to 1.0
  // it is filled with the fog density value from the renderer
  GLfloat c = this->mpLayoutViewer->getPainter()->getFogDensity();

  QDialog* pDialog = new QDialog;
  pDialog->setLayout(new QVBoxLayout(pDialog));
  pDialog->layout()->addWidget(new QLabel("Set the fog density:", pDialog));
  QDoubleSpinBox* pSpinBox = new QDoubleSpinBox(pDialog);
  pSpinBox->setRange(0.0, 1.0);
  pSpinBox->setSingleStep(0.01);
  pSpinBox->setValue(c);
  pDialog->layout()->addWidget(pSpinBox);
  QDialogButtonBox* pBBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, pDialog);
  pDialog->layout()->addWidget(pBBox);
  // connect the buttons
  connect(pBBox, SIGNAL(accepted()), pDialog, SLOT(accept()));
  connect(pBBox, SIGNAL(rejected()), pDialog, SLOT(reject()));

  if (pDialog->exec() == QDialog::Accepted)
    {
      c = pSpinBox->value();
      this->mpLayoutViewer->getPainter()->setFogDensity(c);

      // redraw the GL window
      if (this->mMode == GRAPH_MODE)
        {
          this->mpLayoutViewer->getPainter()->update();
        }
    }

  delete pDialog;
}

/**
 * This slot is triggered when the user wants to change
 * the fog or the highlighting color, depending on the current
 * highlighting mode.
 */
void CQNewMainWindow::changeColorSlot(bool)
{
  // find the correct color for the current mode
  const GLfloat* c = NULL;

  if (this->mpHighlightModeAction->isChecked())
    {
      c = this->mpLayoutViewer->getPainter()->getHighlightColor();
    }
  else
    {
      c = this->mpLayoutViewer->getPainter()->getFogColor();
    }

  // open a color selection dialog
  QColorDialog* pDialog = new QColorDialog(QColor((int)(c[0] * 255.0), (int)(c[1] * 255.0), (int)(c[2] * 255.0), (int)(c[3] * 255.0)), this);

  if (pDialog->exec() == QDialog::Accepted)
    {
      // the dialog has been closed with the OK button
      // so we need to get the new color
      QColor color = pDialog->selectedColor();
      GLfloat newColor[4] = {((GLfloat)color.red()) / 255.0, ((GLfloat)color.green()) / 255.0, ((GLfloat)color.blue()) / 255.0, ((GLfloat)color.alpha()) / 255.0};

      // update the pixmap and the icon for the mpHighlightModeAction
      if (this->mpHighlightModeAction->isChecked())
        {
          this->mpLayoutViewer->getPainter()->setHighlightColor(newColor);
          this->mpHighlightColorPixmap->fill(color);
          this->mpChangeColorAction->setIcon(QIcon(*this->mpHighlightColorPixmap));
        }
      else
        {
          this->mpLayoutViewer->getPainter()->setFogColor(newColor);
          this->mpFogColorPixmap->fill(color);
          this->mpChangeColorAction->setIcon(QIcon(*this->mpFogColorPixmap));
        }

      // redraw the GL window
      if (this->mMode == GRAPH_MODE)
        {
          this->mpLayoutViewer->getPainter()->update();
        }
    }
}
#endif // ELEMENTARY_MODE_DISPLAY

#ifdef COPASI_AUTOLAYOUT
void CQNewMainWindow::redrawNow()
{
  this->mpLayoutViewer->getPainter()->update();
}

/**
 * Creates a CLMetabGlyph for the given CMetab object.
 * If the creation failed, NULL is returned.
 */
CLMetabGlyph* CQNewMainWindow::createMetabGlyph(const std::string& modelobjectkey, double width, double height)
{
  CLMetabGlyph* pResult = new CLMetabGlyph;
  pResult->setDimensions(CLDimensions(width, height));
  pResult->setModelObjectKey(modelobjectkey);
  return pResult;
}

/**
 * Creates a CLCompartmentGlyph for the given size and position
 */
CLCompartmentGlyph* CQNewMainWindow::createCompartmentGlyph(const std::string& modelobjectkey, double x, double y, double width, double height)
{
  CLCompartmentGlyph* pResult = new CLCompartmentGlyph;
  pResult->setPosition(CLPoint(x, y));
  pResult->setDimensions(CLDimensions(width, height));
  pResult->setModelObjectKey(modelobjectkey);
  return pResult;
}

/**
 * Creates a new reaction glyph with the given size.
 */
CLReactionGlyph* CQNewMainWindow::createReactionGlyph(const std::string& modelobjectkey, double x, double y, double length)
{
  CLReactionGlyph* pResult = new CLReactionGlyph;
  //pResult->setPosition(CLPoint(x,y));
  //pResult->setDimensions(CLDimensions(width, height));
  pResult->setModelObjectKey(modelobjectkey);
  pResult->getCurve().addCurveSegment(CLLineSegment(CLPoint(x, y), CLPoint(x + length, y)));
  return pResult;
}

/**
 * Creates a CLTextGlyph for the given graphical object keys and size.
 */
CLTextGlyph* CQNewMainWindow::createTextGlyph(const std::string& modelobjectkey, const std::string& objectkey, double width, double height)
{
  CLTextGlyph* pResult = new CLTextGlyph;
  pResult->setDimensions(CLDimensions(width, height));
  pResult->setGraphicalObjectKey(objectkey);
  pResult->setModelObjectKey(modelobjectkey);
  return pResult;
}

/**
 * Creates a CLMetabReferenceGlyph for the given endpoints.
 */
CLMetabReferenceGlyph* CQNewMainWindow::createMetabReferenceGlyph(const std::string& modelobjectkey, const std::string& metabglyphkey, CLMetabReferenceGlyph::Role role, double x1, double y1, double x2, double y2)
{
  CLMetabReferenceGlyph* pResult = new CLMetabReferenceGlyph;
  pResult->setModelObjectKey(modelobjectkey);
  pResult->setMetabGlyphKey(metabglyphkey);
  pResult->setRole(role);
  // set the curve
  pResult->getCurve().addCurveSegment(CLLineSegment(CLPoint(x1, y1), CLPoint(x2, y2)));
  return pResult;
}

/**
 * This method creates a random layout using the elements
 * in the compartments, reactions, species and side species
 * containers.
 */
void CQNewMainWindow::createRandomLayout(const std::set<const CCompartment*>& compartments,
    const std::set<const CReaction*>& reactions,
    const std::set<const CMetab*>& metabs,
    const std::set<const CMetab*>& sideMetabs
                                        )
{
  const double REACTION_SIZE = 20.0;
  std::set<CLMetabGlyph*> duplicateNodes;

  // we need a multimap that stores the association between a metabolite and it's glyph
  std::map<const CMetab*, CLMetabGlyph*> metabGlyphMap;

  QFont font = QFont("Helvetica", 16);
  QFontMetrics metrics = QFontMetrics(font);
  QRect textBounds;

  // create a species glyph for each species in metabs
  std::set<const CMetab*>::const_iterator metabIt = metabs.begin(), metabEndit = metabs.end();
  CLMetabGlyph* pMetabGlyph = NULL;
  CLTextGlyph* pTextGlyph = NULL;
  double width = 0.0;
  double height = 0.0;
  double totalarea = 0.0;
  // store the area for each compartment in a map
  std::map<const CCompartment*, double> areaMap;
  // a map that associated text glpyhs with metab glyphs
  // we need this to later place the text glyph
  // on top of the metab glyph
  std::map<CLMetabGlyph*, CLTextGlyph*> textGlyphMap;
  std::map<CLMetabGlyph*, CLTextGlyph*> textGlyphMap2;
  // we need to store which compartment contains which metabolite glyphs
  std::map<const CCompartment*, std::set<CLMetabGlyph*> > compmetmap;

  while (metabIt != metabEndit)
    {
      textBounds = metrics.boundingRect((*metabIt)->getObjectName().c_str());
      width = (double)textBounds.width();
      height = (double)textBounds.height();

      if (width < height)
        {
          width = height;
        }

      // make the metab glyph 120% the size of the text
      pMetabGlyph = CQNewMainWindow::createMetabGlyph((*metabIt)->getKey(), width + 4, height + 4);
      assert(pMetabGlyph != NULL);
      metabGlyphMap[*metabIt] = pMetabGlyph;
      this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
      pTextGlyph = CQNewMainWindow::createTextGlyph((*metabIt)->getKey(), pMetabGlyph->getKey(), width, height);
      assert(pTextGlyph != NULL);
      this->mpCurrentLayout->addTextGlyph(pTextGlyph);
      textGlyphMap[pMetabGlyph] = pTextGlyph;
      textGlyphMap2[pMetabGlyph] = pTextGlyph;
      totalarea += width * height * 1.2 * 1.2;

      if (areaMap.find((*metabIt)->getCompartment()) != areaMap.end())
        {
          areaMap[(*metabIt)->getCompartment()] += width * height * 1.2 * 1.2;
        }
      else
        {
          areaMap[(*metabIt)->getCompartment()] = width * height * 1.2 * 1.2;
        }

      if (compmetmap.find((*metabIt)->getCompartment()) != compmetmap.end())
        {
          compmetmap[(*metabIt)->getCompartment()].insert(pMetabGlyph);
        }
      else
        {
          std::set<CLMetabGlyph*> s;
          s.insert(pMetabGlyph);
          compmetmap[(*metabIt)->getCompartment()] = s;
        }

      ++metabIt;
    }

  // now we need a random number generator
  CRandom* pRandom = CRandom::createGenerator(CRandom::mt19937, CRandom::getSystemSeed());

  // we increase the total area by a factor of 6
  totalarea *= 6;
  double totalsidelength = sqrt(totalarea);
  // create a compartment glyph for each compartment in compartments
  std::map<const CCompartment*, CLBoundingBox> compartmentAreaMap;

  if (!compartments.empty())
    {
      double compartmentarea = 0.0;
      std::set<const CCompartment*>::const_iterator compIt = compartments.begin(), compEndit = compartments.end();
      CLCompartmentGlyph* pCompartmentGlyph = NULL;
      // position variables
      double boundX, boundY;
      double x, y;

      // we need to remember the compartment glyph positions and sizes
      // so that we can later place the reaction in the correct area
      while (compIt != compEndit)
        {
          if (areaMap.find(*compIt) != areaMap.end())
            {
              compartmentarea = areaMap[*compIt];
              // we increase the size by a factor of 5
              compartmentarea *= 5.0;
              // this space has been filled
              totalarea -= compartmentarea;
            }
          else
            {
              compartmentarea = 2500.0;
            }

          double sidelength = sqrt(compartmentarea);
          // position the compartment somwehere within the total area
          // so that is will be completely within the total area
          boundX = totalsidelength - sidelength;
          boundY = boundX;
          x = boundX * pRandom->getRandomCC();
          y = boundY * pRandom->getRandomCC();
          pCompartmentGlyph = CQNewMainWindow::createCompartmentGlyph((*compIt)->getKey(), x, y, sidelength, sidelength);
          this->mpCurrentLayout->addCompartmentGlyph(pCompartmentGlyph);
          // we need to store the areas and the positions of the compartments as well as the values for the "external"
          // space so that we can later place the reactions within the appropriate space
          compartmentAreaMap[*compIt] = pCompartmentGlyph->getBoundingBox();

          // we leave 5% space from the border of the compartment
          x += sidelength * 0.05;
          y += sidelength * 0.05;
          sidelength *= 0.95;

          // now we know the size of the compartment
          // and we can place the species glyphs within that area
          if (compmetmap.find(*compIt) != compmetmap.end())
            {
              std::set<CLMetabGlyph*>::iterator gIt = compmetmap[*compIt].begin(), gEndit = compmetmap[*compIt].end();

              while (gIt != gEndit)
                {
                  // place the metab glyph and the associated text glyph
                  double xPos = x + pRandom->getRandomCC() * sidelength - (*gIt)->getDimensions().getWidth();
                  double yPos = y + pRandom->getRandomCC() * sidelength - (*gIt)->getDimensions().getHeight();
                  (*gIt)->setPosition(CLPoint(xPos, yPos));
                  pTextGlyph = textGlyphMap[*gIt];
                  assert(pTextGlyph != NULL);
                  pTextGlyph->setPosition(CLPoint(xPos + ((*gIt)->getDimensions().getWidth() - pTextGlyph->getDimensions().getWidth()) * 0.5, yPos + ((*gIt)->getDimensions().getHeight() - pTextGlyph->getDimensions().getHeight()) * 0.5));
                  // if we remove the species that have already been placed, we can
                  // later determine which elements are not layed out in a compartment
                  textGlyphMap.erase(textGlyphMap.find(*gIt));
                  ++gIt;
                }
            }

          ++compIt;
        }
    }

  // now we calculate the remaining area and place the rest of the species glyphs
  // (those that remain in the textGlyphMap)
  // we place the remaining species glyphs in a sidelength * sidelength area
  double externalsidelength = sqrt(5 * totalarea);
  std::map<CLMetabGlyph*, CLTextGlyph*>::iterator gIt = textGlyphMap.begin(), gEndit = textGlyphMap.end();
  // we place these items beneath the other compartments
  double x = totalsidelength;

  while (gIt != gEndit)
    {
      double xPos = x + pRandom->getRandomCC() * (externalsidelength - gIt->first->getDimensions().getWidth());
      double yPos = pRandom->getRandomCC() * (externalsidelength - gIt->first->getDimensions().getHeight());
      gIt->first->setPosition(CLPoint(xPos, yPos));
      xPos += 0.5 * (gIt->first->getDimensions().getWidth() - gIt->second->getDimensions().getWidth());
      yPos += 0.5 * (gIt->first->getDimensions().getHeight() - gIt->second->getDimensions().getHeight());
      gIt->second->setPosition(CLPoint(xPos, yPos));
      ++gIt;
    }

  //
  //
  // create a reaction glyph for each reaction in reactions
  // species that appear in sideMetabs are duplicated for each reaction
  // we store the end iterator for the side species set because we need it many times
  std::set<const CMetab*>::const_iterator sideSpeciesEnd = sideMetabs.end();
  std::set<const CReaction*>::const_iterator reactIt = reactions.begin(), reactEndit = reactions.end();

  while (reactIt != reactEndit)
    {
      // right now a reaction will be represented by a line with REACTION_SIZE length
      // and the species references will be connected to middle of
      // the left and right side of the box
      // substrates will be left and products will be right.
      //
      // If the reaction is a single compartment reaction, we place it inside the compartments area,
      // else we place it in the outside area
      double xPos = 0.0;
      double yPos = 0.0;

      if ((*reactIt)->getCompartmentNumber() > 1)
        {
          // put it in the "external" space
          xPos = x + pRandom->getRandomCC() * (externalsidelength - 20.0);
          yPos = pRandom->getRandomCC() * externalsidelength - 20.0;
        }
      else
        {
          // we need the compartment bounding box
          assert((*reactIt)->getChemEq().getSubstrates().size() > 0 || (*reactIt)->getChemEq().getProducts().size());
          const CCompartment* pCompartment = NULL;

          if ((*reactIt)->getChemEq().getProducts().size() == 0)
            {
              pCompartment = (*reactIt)->getChemEq().getSubstrates()[0]->getMetabolite()->getCompartment();
            }
          else
            {
              pCompartment = (*reactIt)->getChemEq().getProducts()[0]->getMetabolite()->getCompartment();
            }

          std::map<const CCompartment*, CLBoundingBox>::const_iterator pos = compartmentAreaMap.find(pCompartment);

          if (pos != compartmentAreaMap.end())
            {
              // put it in the compartment
              xPos = pos->second.getPosition().getX() + pRandom->getRandomCC() * (pos->second.getDimensions().getWidth() - 20.0);
              yPos = pos->second.getPosition().getY() + pRandom->getRandomCC() * (pos->second.getDimensions().getHeight() - 20.0);
            }
          else
            {
              // put it in the "external" space
              xPos = x + pRandom->getRandomCC() * (externalsidelength - 20.0);
              yPos = pRandom->getRandomCC() * (externalsidelength - 20.0);
            }
        }

      CLReactionGlyph* pReactionGlyph = CQNewMainWindow::createReactionGlyph((*reactIt)->getKey(), xPos, yPos, REACTION_SIZE);
      this->mpCurrentLayout->addReactionGlyph(pReactionGlyph);
      // create the species reference glyph and duplicate side elements
      //
      // if the same species occurs several times as substrate and or product in a reaction
      // we make a copy even it is not in the list of side species
      std::map<const CMetab*, const CLMetabGlyph*> linkedElements;
      const CCopasiVector < CChemEqElement >& substrates = (*reactIt)->getChemEq().getSubstrates();
      CCopasiVector<CChemEqElement>::const_iterator sIt = substrates.begin(), sEndit = substrates.end();
      const CMetab* pMetab = NULL;

      while (sIt != sEndit)
        {
          // check is the species reference is to be created at all
          pMetab = (*sIt)->getMetabolite();
          assert(pMetab != NULL);

          if (metabs.find(pMetab) != metabs.end())
            {
              CLMetabGlyph* pMetabGlyph = NULL;

              // check if a reference to this species already has been created
              if (linkedElements.find(pMetab) != linkedElements.end())
                {
                  // we make a copy without putting the original in the duplicatedNodes set
                  // becasue it might be used in another reaction as well, so we don't need
                  // to delete it
                  // also copy the corresponding text glyph
                  pTextGlyph = textGlyphMap2[pMetabGlyph];
                  assert(pTextGlyph != NULL);
                  pTextGlyph = new CLTextGlyph(*pTextGlyph);
                  pMetabGlyph = new CLMetabGlyph(*(linkedElements.find(pMetab)->second));
                  this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
                  pTextGlyph->setGraphicalObjectKey(pMetabGlyph->getKey());
                  this->mpCurrentLayout->addTextGlyph(pTextGlyph);
                }
              else
                {
                  // is the species is in the side species map, so we have to duplicate the species glyph
                  // and put the original node in the duplicatedNodes set
                  //
                  // first we have to find the original species
                  pMetabGlyph = metabGlyphMap[pMetab];
                  CLMetabReferenceGlyph::Role role = CLMetabReferenceGlyph::SUBSTRATE;

                  if (sideMetabs.find(pMetab) != sideSpeciesEnd)
                    {
                      // we make a copy and place the original in the duplicateNodesList
                      duplicateNodes.insert(pMetabGlyph);
                      // also copy the corresponding text glyph
                      pTextGlyph = textGlyphMap2[pMetabGlyph];
                      assert(pTextGlyph != NULL);
                      pTextGlyph = new CLTextGlyph(*pTextGlyph);
                      pMetabGlyph = new CLMetabGlyph(*pMetabGlyph);
                      this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
                      pTextGlyph->setGraphicalObjectKey(pMetabGlyph->getKey());
                      this->mpCurrentLayout->addTextGlyph(pTextGlyph);
                      role = CLMetabReferenceGlyph::SIDESUBSTRATE;
                      // TODO maybe change the position of the copy, but this is not really
                      // TODO necessary
                    }

                  // create the species reference glyph
                  // connect it to the middle of the left side of the reaction
                  double metabX = pMetabGlyph->getPosition().getX();
                  double metabY = pMetabGlyph->getPosition().getY();

                  if (metabX < xPos)
                    {
                      // the metabolite is to the left, so
                      // we attach at the right side
                      metabX += pMetabGlyph->getDimensions().getWidth();
                    }

                  if (metabY < yPos)
                    {
                      // the metabolite is above, so we attach to the bottom
                      metabY += pMetabGlyph->getDimensions().getHeight();
                    }

                  // create a species reference edge from the reaction (xPos,yPos+5) to metabX,metabY
                  CLMetabReferenceGlyph* pRefGlyph = CQNewMainWindow::createMetabReferenceGlyph((*sIt)->getKey(), pMetabGlyph->getKey(), role, xPos, yPos, metabX, metabY);
                  pReactionGlyph->addMetabReferenceGlyph(pRefGlyph);
                }
            }

          ++sIt;
        }

      const CCopasiVector < CChemEqElement >& products = (*reactIt)->getChemEq().getProducts();

      sIt = products.begin();

      sEndit = products.end();

      while (sIt != sEndit)
        {
          // check if the species reference is to be created at all
          pMetab = (*sIt)->getMetabolite();
          assert(pMetab != NULL);

          if (metabs.find(pMetab) != metabs.end())
            {
              CLMetabGlyph* pMetabGlyph = NULL;

              // check if a reference to this species already has been created
              if (linkedElements.find(pMetab) != linkedElements.end())
                {
                  // we make a copy without putting the original in the duplicatedNodes set
                  // becasue it might be used in another reaction as well, so we don't need
                  // to delete it
                  // also copy the corresponding text glyph
                  pTextGlyph = textGlyphMap2[pMetabGlyph];
                  assert(pTextGlyph != NULL);
                  pTextGlyph = new CLTextGlyph(*pTextGlyph);
                  pMetabGlyph = new CLMetabGlyph(*(linkedElements.find(pMetab)->second));
                  this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
                  pTextGlyph->setGraphicalObjectKey(pMetabGlyph->getKey());
                  this->mpCurrentLayout->addTextGlyph(pTextGlyph);
                }
              else
                {
                  // is the species is in the side species map, so we have to duplicate the species glyph
                  // and put the original node in the duplicatedNodes set
                  //
                  // first we have to find the original species
                  pMetabGlyph = metabGlyphMap[pMetab];
                  CLMetabReferenceGlyph::Role role = CLMetabReferenceGlyph::PRODUCT;

                  if (sideMetabs.find(pMetab) != sideSpeciesEnd)
                    {
                      // we make a copy and place the original in the duplicateNodesList
                      duplicateNodes.insert(pMetabGlyph);
                      // also copy the corresponding text glyph
                      pTextGlyph = textGlyphMap2[pMetabGlyph];
                      assert(pTextGlyph != NULL);
                      pTextGlyph = new CLTextGlyph(*pTextGlyph);
                      pMetabGlyph = new CLMetabGlyph(*pMetabGlyph);
                      this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
                      pTextGlyph->setGraphicalObjectKey(pMetabGlyph->getKey());
                      this->mpCurrentLayout->addTextGlyph(pTextGlyph);
                      role = CLMetabReferenceGlyph::SIDEPRODUCT;
                      // TODO maybe change the position of the copy, but this is not really
                      // TODO necessary
                    }

                  // create the species reference glyph
                  // connect it to the middle of the left side of the reaction
                  double metabX = pMetabGlyph->getPosition().getX();
                  double metabY = pMetabGlyph->getPosition().getY();

                  if (metabX < xPos + 10)
                    {
                      // the metabolite is to the left, so
                      // we attach at the right side
                      metabX += pMetabGlyph->getDimensions().getWidth();
                    }

                  if (metabY < yPos)
                    {
                      // the metabolite is above, so we attach to the bottom
                      metabY += pMetabGlyph->getDimensions().getHeight();
                    }

                  // create a species reference edge from the reaction (xPos,yPos+5) to metabX,metabY
                  CLMetabReferenceGlyph* pRefGlyph = CQNewMainWindow::createMetabReferenceGlyph((*sIt)->getKey(), pMetabGlyph->getKey(), role, xPos + REACTION_SIZE, yPos, metabX, metabY);
                  pReactionGlyph->addMetabReferenceGlyph(pRefGlyph);
                }
            }

          ++sIt;
        }

      // is a modifier is also substrate or product in a reaction, don't duplicate it for the modifier
      // but just add an edge to the exisiting node
      const CCopasiVector < CChemEqElement >& modifiers = (*reactIt)->getChemEq().getModifiers();
      sIt = modifiers.begin();
      sEndit = modifiers.end();

      while (sIt != sEndit)
        {
          // check if the species reference is to be created at all
          pMetab = (*sIt)->getMetabolite();
          assert(pMetab != NULL);

          if (metabs.find(pMetab) != metabs.end())
            {
              CLMetabGlyph* pMetabGlyph = NULL;

              // is the species is in the side species map, so we have to duplicate the species glyph
              // and put the original node in the duplicatedNodes set
              //
              // first we have to find the original species
              pMetabGlyph = metabGlyphMap[pMetab];
              CLMetabReferenceGlyph::Role role = CLMetabReferenceGlyph::MODIFIER;

              if (sideMetabs.find(pMetab) != sideSpeciesEnd)
                {
                  // we make a copy and place the original in the duplicateNodesList
                  duplicateNodes.insert(pMetabGlyph);
                  // also copy the corresponding text glyph
                  pTextGlyph = textGlyphMap2[pMetabGlyph];
                  assert(pTextGlyph != NULL);
                  pTextGlyph = new CLTextGlyph(*pTextGlyph);
                  pMetabGlyph = new CLMetabGlyph(*pMetabGlyph);
                  pTextGlyph->setGraphicalObjectKey(pMetabGlyph->getKey());
                  this->mpCurrentLayout->addTextGlyph(pTextGlyph);
                  this->mpCurrentLayout->addMetaboliteGlyph(pMetabGlyph);
                  // TODO maybe change the position of the copy, but this is not really
                  // TODO necessary
                }

              // create the species reference glyph
              // connect it to the middle of the left side of the reaction
              double metabX = pMetabGlyph->getPosition().getX();
              double metabY = pMetabGlyph->getPosition().getY();

              if (metabX < xPos + 5)
                {
                  // the metabolite is to the left, so
                  // we attach at the right side
                  metabX += pMetabGlyph->getDimensions().getWidth();
                }

              // create a species reference edge from the reaction (xPos,yPos+5) to metabX,metabY
              CLMetabReferenceGlyph* pRefGlyph = CQNewMainWindow::createMetabReferenceGlyph((*sIt)->getKey(), pMetabGlyph->getKey(), role, metabX, metabY, xPos + 0.5 * REACTION_SIZE, yPos);
              pReactionGlyph->addMetabReferenceGlyph(pRefGlyph);
            }

          ++sIt;
        }

      ++reactIt;
    }

  //
  // delete the duplicated nodes again
  std::set<CLMetabGlyph*>::iterator duplicateIt = duplicateNodes.begin(), duplicateEndit = duplicateNodes.end();
  CCopasiVector<CLMetabGlyph>& metabGlyphs = this->mpCurrentLayout->getListOfMetaboliteGlyphs();
  CCopasiVector<CLTextGlyph>& textGlyphs = this->mpCurrentLayout->getListOfTextGlyphs();
  bool r = false;

  while (duplicateIt != duplicateEndit)
    {
      pMetabGlyph = *duplicateIt;
      pTextGlyph = textGlyphMap2[pMetabGlyph];
      assert(pTextGlyph != NULL);
      // the glyph has to be in there
      r = textGlyphs.remove(pTextGlyph);
      assert(r == true);

      if (r == true)
        {
          delete pTextGlyph;
        }

      r = metabGlyphs.remove(pMetabGlyph);
      // the glyph has to be in there
      assert(r == true);

      // TODO this check is only to work around a bug
      // TODO I haven't fixed yet.
      // TODO I prefer a memory leak over a crash
      if (r == true)
        {
          delete pMetabGlyph;
        }

      ++duplicateIt;
    }

  duplicateNodes.clear();

  delete pRandom;
  // determine and set the layout dimensions
  CLBoundingBox box = this->mpCurrentLayout->calculateBoundingBox();
  this->mpCurrentLayout->setDimensions(CLDimensions(box.getDimensions().getWidth() + 30.0, box.getDimensions().getHeight() + 30.0));
}

/**
 * Creates a spring layout.
 * The method takes the number of iterations for the
 * layout algorithm and an update interval which tells the algorithm
 * how often to update the display.
 * A value of -1 means that the update of the display is only done once
 * at the end.
 */
void CQNewMainWindow::createSpringLayout(int numIterations, int updateInterval)
{
  // reset the stop flag
  this->mStopLayout = false;
  disconnect(this->mpStopLayoutAction, SIGNAL(triggered()), this, SLOT(slotRunSpringLayout()));
  this->mpStopLayoutAction->setToolTip("stop spring layout algorithm");
  connect(this->mpStopLayoutAction, SIGNAL(triggered()), this, SLOT(slotStopClicked()));
  // enable the stop button
  this->mpStopLayoutAction->setEnabled(true);
  this->mpStopLayoutAction->setIcon(QPixmap(layout_stop_xpm));

  // now we create the random layout
  if (updateInterval < 0) updateInterval = -1;

  bool doUpdate = (updateInterval != -1);

  if (numIterations > 0 && this->mpCurrentLayout != NULL &&
      (this->mpCurrentLayout->getListOfCompartmentGlyphs().size() > 0 || this->mpCurrentLayout->getListOfMetaboliteGlyphs().size() > 0))
    {
      // create the spring layout
      CCopasiSpringLayout l(this->mpCurrentLayout);
      l.createVariables();
      CLayoutEngine le(&l, false);
      QAbstractEventDispatcher* pDispatcher = QAbstractEventDispatcher::instance();
      int i = 0;
      double pot, oldPot = -1.0;

      for (; (i < numIterations) && (this->mStopLayout) == false; ++i)
        {
          pot = le.step();

          if (pot == 0.0 || fabs((pot - oldPot) / pot) < 1e-9)
            {
              break;
            }
          else
            {
              oldPot = pot;
            }

          if (doUpdate && (i % updateInterval == 0))
            {
              l.finalizeState(); //makes the layout ready for drawing;
              // redraw
              slotCalculateDimensions();
            }

          if (pDispatcher->hasPendingEvents())
            {
              pDispatcher->processEvents(QEventLoop::AllEvents);
            }
        }

      // redraw the layout
      l.finalizeState(); //makes the layout ready for drawing;

      if (!doUpdate || (i % updateInterval != 0))
        {
          slotCalculateDimensions();
        }
    }

  // disable the stop button
  //this->mpStopLayoutAction->setEnabled(false);
  disconnect(this->mpStopLayoutAction, SIGNAL(triggered()), this, SLOT(slotStopClicked()));
  connect(this->mpStopLayoutAction, SIGNAL(triggered()), this, SLOT(slotRunSpringLayout()));
  this->mpStopLayoutAction->setIcon(QPixmap(layout_start_xpm));
  this->mpStopLayoutAction->setToolTip("Run Spring Layout Algorithm");
}

/**
 * This slot is called when the stop button is presed.
 * It notifies the layout method to stop the spring layout iterations.
 */
void CQNewMainWindow::slotStopClicked()
{
  this->mStopLayout = true;
}

void CQNewMainWindow::slotRunSpringLayout()
{
  this->createSpringLayout(1000, 1);
}

QRectF getBounds(const std::vector<CCopasiSpringLayout::UpdateAction>& updates)
{
  QRectF result(1000, 1000, -1000, -1000);
  std::vector<CCopasiSpringLayout::UpdateAction>::const_iterator it, itEnd = updates.end();

  for (it = updates.begin(); it != itEnd; ++it)
    {
      switch (it->mAction)
        {
          case CCopasiSpringLayout::UpdateAction::COMPARTMENT_4V:
          {
            CLCompartmentGlyph* current = ((CLCompartmentGlyph*)(it->mpTarget));
            result.setLeft(qMin(result.left(), current->getX()));
            result.setTop(qMin(result.top(), current->getY()));
            result.setRight(qMax(result.right(), current->getX() + current->getWidth()));
            result.setBottom(qMax(result.bottom(), current->getY() + current->getHeight()));
          }
          break;

          case CCopasiSpringLayout::UpdateAction::SPECIES_2V:
          {
            CLMetabGlyph* current = ((CLMetabGlyph*)(it->mpTarget));
            result.setLeft(qMin(result.left(), current->getX()));
            result.setTop(qMin(result.top(), current->getY()));
            result.setRight(qMax(result.right(), current->getX() + current->getWidth()));
            result.setBottom(qMax(result.bottom(), current->getY() + current->getHeight()));
          }
          break;

          case CCopasiSpringLayout::UpdateAction::REACTION_2V:
          {
            CLReactionGlyph* current = ((CLReactionGlyph*)(it->mpTarget));
            result.setLeft(qMin(result.left(), current->getX()));
            result.setTop(qMin(result.top(), current->getY()));
            result.setRight(qMax(result.right(), current->getX() + current->getWidth()));
            result.setBottom(qMax(result.bottom(), current->getY() + current->getHeight()));
          }
          break;

          default:
            break;
        };
    }

  return result;
}

void CQNewMainWindow::slotCalculateDimensions()
{
  if (this->mpCurrentLayout == NULL ||
      (this->mpCurrentLayout->getListOfCompartmentGlyphs().size() == 0 && this->mpCurrentLayout->getListOfMetaboliteGlyphs().size() == 0))
    return;

  mpLayoutViewer->getPainter()->calculateAndAssignBounds(mpCurrentLayout);
  // redraw
  this->redrawNow();
}

void CQNewMainWindow::slotRunRandomizeLayout()
{
  if (this->mpCurrentLayout == NULL ||
      (this->mpCurrentLayout->getListOfCompartmentGlyphs().size() == 0 && this->mpCurrentLayout->getListOfMetaboliteGlyphs().size() == 0))
    return;

  // create the spring layout
  CCopasiSpringLayout l(this->mpCurrentLayout);
  l.createVariables();

  CRandom* pRandom = CRandom::createGenerator(CRandom::mt19937, CRandom::getSystemSeed());

  const std::vector<CCopasiSpringLayout::UpdateAction>& updateActions = l.getUpdateActions();
  std::vector<double> initialValues = l.getInitialValues();

  std::vector<CCopasiSpringLayout::UpdateAction>::const_iterator it, itEnd = updateActions.end();

  double right = mpCurrentLayout->getDimensions().getWidth();
  double bottom = mpCurrentLayout->getDimensions().getHeight();
  double left = right / 2.0;
  double top = 0.0;//bottom/2.0;

  for (it = updateActions.begin(); it != itEnd; ++it)
    {
      switch (it->mAction)
        {
          case CCopasiSpringLayout::UpdateAction::COMPARTMENT_4V:
          case CCopasiSpringLayout::UpdateAction::SPECIES_2V:
            //case CCopasiSpringLayout::UpdateAction::REACTION_2V:
            initialValues[it->mIndex1] = left + pRandom->getRandomCC() * right;
            initialValues[it->mIndex2] = top + pRandom->getRandomCC() * bottom;
            break;

          default:
            break;
        };
    }

  l.setState(initialValues);
  l.finalizeState(); //makes the layout ready for drawing;

  slotCalculateDimensions();
}

#endif // COPASI_AUTOLAYOUT

void CQNewMainWindow::closeEvent(QCloseEvent * event)
{
#ifdef COPASI_AUTOLAYOUT
  this->slotStopClicked();
#endif // COPASI_AUTOLAYOUT
  removeFromMainWindow();
  this->QMainWindow::closeEvent(event);
}
