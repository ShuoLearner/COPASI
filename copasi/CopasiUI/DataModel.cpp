#include "DataModel.h"

DataModel::DataModel(char* fileName)
{
  this->populateData(fileName);
  model = NULL;
  trajectorytask = NULL;
  steadystatetask = NULL;
  scantask = NULL;
  reportdefinitions = NULL;
}

Folder* DataModel::searchFolderList(int id)
{
  for (Folder* f = folderList.first(); f; f = folderList.next())
    if (f->getID() == id)
      return f;

  return NULL;
}

Node<Folder> * DataModel::addData(Folder* parent, Folder* child)
{
  myTree.addSibling(child, parent);
  folderList.append(child);
  return /*last = */ myTree.search(child, myTree.getRoot());    //propably needs to return
}

void DataModel::removeData(Folder* i)
{
  Node<Folder>* node = myTree.search(i, myTree.getRoot());

  if (node)
    {
      //last = node;
      //notify(DELETE);

      myTree.deleteNode(i);
      folderList.remove(i);
    }

  //last = NULL;
}

void DataModel::removeAllChildren(Folder* f)
{
  Node<Folder>* node = myTree.search(f, myTree.getRoot());
  Folder* fff;

  if (!node->child) return;

  while (node->child)
    {
      fff = node->child->info;
      removeData(node->child->info);
      delete fff;
    }
}

void DataModel::createModel(const char* fileName)
{
  pdelete(model);
  model = new CModel();
  Copasi->pModel = model;
  searchFolderList(1)->setObjectKey(model->getKey());

  pdelete(steadystatetask);
  steadystatetask = new CSteadyStateTask();
  steadystatetask->getProblem()->setModel(model);
  searchFolderList(21)->setObjectKey(steadystatetask->getKey());

  pdelete(trajectorytask);
  trajectorytask = new CTrajectoryTask();
  trajectorytask->getProblem()->setModel(model);
  searchFolderList(23)->setObjectKey(trajectorytask->getKey());  //23=Time course

  pdelete(scantask);
  scantask = new CScanTask();
  scantask->getProblem()->setModel(model);
  searchFolderList(32)->setObjectKey(scantask->getKey());

  pdelete(reportdefinitions);
  reportdefinitions = new CReportDefinitionVector();
  searchFolderList(43)->setObjectKey(reportdefinitions->getKey());

  saveModel(fileName);
}

void DataModel::loadModel(const char* fileName)
{
  pdelete(model);
  CReadConfig inbuf(fileName);
  model = new CModel();
  model->load(inbuf);
  searchFolderList(1)->setObjectKey(model->getKey());

  pdelete(steadystatetask);
  steadystatetask = new CSteadyStateTask();
  steadystatetask->load(inbuf);
  searchFolderList(21)->setObjectKey(steadystatetask->getKey());

  pdelete(trajectorytask);
  trajectorytask = new CTrajectoryTask();
  trajectorytask->load(inbuf);
  searchFolderList(23)->setObjectKey(trajectorytask->getKey()); //23=Time course

  pdelete(scantask);
  scantask = new CScanTask();
  scantask->getProblem()->setModel(model);
  // future work  scantask->load(inbuf);
  searchFolderList(32)->setObjectKey(scantask->getKey());

  pdelete(reportdefinitions);
  reportdefinitions = new CReportDefinitionVector();
  reportdefinitions->load(inbuf);
  searchFolderList(43)->setObjectKey(reportdefinitions->getKey());

  Copasi->pOutputList->load(inbuf);

  //  steadystatetask->compile();
}

void DataModel::saveModel(const char* fileName)
{
  if (fileName == NULL) return;

  CCopasiXML XML;

  std::ofstream os(fileName);

  XML.setModel(*model);
  XML.save(os);
}

void DataModel::populateData(char* fileName)
{
  QString str = QString("Folder ");
  Folder *f = new Folder(0, str);
  f->setID(0, true);
  myTree.addRoot(f);
  folderList.append(f);

  // const int MAX = 80;
  std::string str1;

  std::ifstream in(fileName);

  while (!in.eof())
    {
      in >> str1;
      if (str1 == "")
        break;

      QString data(str1.c_str());

      int first = data.find(':');

      int second = data.find(':', first + 1);

      int parentId = data.mid(0, first).toInt();

      int myId = data.mid(first + 1, second - first - 1).toInt();

      QString myStr = data.mid(second + 1, data.length() - second - 1);

      str = QString(myStr);

      Folder* parent = this->searchFolderList(parentId);

      Folder* f1;

      f1 = (parentId == 0) ? new Folder(0, str) : new Folder(parent, str);

      f1->setID(myId, true);

      myTree.addSibling(f1, parent);

      folderList.append(f1);
    }

  in.close();
}
