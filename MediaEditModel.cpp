#include "MediaEditModel.h"

#include <QSize>
#include <QIcon>
#include <QDateTime>
#include <QRegExp>



MediaEditModel::MediaEditModel(QObject *parent) 
	: QAbstractItemModel(parent)
{
	
}

int MediaEditModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return 3;
}

QVariant MediaEditModel::data(const QModelIndex &index, int role) const 
{
    if (!index.isValid())
        return QVariant();

	if (role == Qt::SizeHintRole){
	   return QSize(15,15);
	}

	if (role == Qt::DecorationRole) {
		if (index.column() == (int)eHeader::eName) {
			auto item = static_cast<QFileInfo*>(index.internalPointer());
			if (item == nullptr)
				return QVariant();	

            if (item->completeBaseName().isEmpty()) {
                QIcon icon(":/FilesNames/Resources/arrow.png");
                return icon;
            }
			return	icon_provider_.icon(*item);
		}
		return QVariant();
	}
				
	if (role == Qt::DisplayRole) {
		auto item = static_cast<QFileInfo*>(index.internalPointer());
		if (item == nullptr)
			return QVariant();

		switch((eHeader)index.column()){
		case eHeader::eName: {
            if (item->completeBaseName().isEmpty()) {
                return "..";
            }
			if (item->isDir()) {
				return item->completeBaseName();		
			}
			if (item->isFile()) {
				return item->fileName();
			}

		}
	
		case eHeader::ePriority:
            return item->created();

		case eHeader::eNote:	{
			if (item->isDir()) {
				return "<DIR>";		
			}
			if (item->isFile()) {
				return getFileSize(item->size());
			}
			return QVariant();
		}
		}
	}
	return QVariant();
}

Qt::ItemFlags MediaEditModel::flags(const QModelIndex &index) const
{
	return Qt::ItemIsSelectable | Qt::ItemIsEnabled /*| Qt::ItemIsEditable*/;
}

QVariant MediaEditModel::headerData(int section, Qt::Orientation orientation, int role) const 
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch ((eHeader)section) {
            case eHeader::eName:
				return QString("File Name");
            case eHeader::eNote:
                return QString("Size");
			case eHeader::ePriority:
				return QString("Date");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QModelIndex MediaEditModel::index(int row, int column, const QModelIndex &parent) const 
{
 		if (!hasIndex(row, column, parent))
 			return QModelIndex{};

		auto childItem = const_cast<QFileInfo*>(&(list_.at(row)));
		if (childItem)
			return createIndex(row, column, childItem);
   		else
			return QModelIndex{};
}

QModelIndex MediaEditModel::parent(const QModelIndex &child) const 
{
    Q_UNUSED(child);
    return QModelIndex();
}

int MediaEditModel::rowCount(const QModelIndex &parent) const 
{

	if (!parent.isValid())
		return list_.count();		

    return 0;
}

bool MediaEditModel::setData(const QModelIndex & index, const QVariant & value, int role) 
{
	Q_UNUSED(index);
	Q_UNUSED(role);
	Q_UNUSED(value);
	return false;
}

void MediaEditModel::setData(const QString & folder) 
{
	QDir dir(folder);

	if (!dir.exists()) {
		return;
	}
	dir_ = dir;		  
	refresh();
}

void MediaEditModel::refresh()
{

	list_.clear();
    list_ = dir_.entryInfoList(QDir::AllEntries | QDir::NoDotDot, QDir::DirsFirst | QDir::IgnoreCase);
	for (int i = 0; i < list_.count(); i++) {
		beginInsertRows(QModelIndex(),i,i); 
		QFileInfo *childItem = const_cast<QFileInfo*>(&(list_.at(i)));
		createIndex(i, 0, childItem);	
		endInsertRows();
	}

	beginResetModel();
	endResetModel();
}

QFileInfo * MediaEditModel::getPropertyItem(const QModelIndex &index) 
{
	if (!index.isValid())
		return nullptr;

	QFileInfo * childItem = static_cast<QFileInfo*>(index.internalPointer());
	return childItem;
}

QString MediaEditModel::getFileSize(quint64 size) const 
{
	if (size > 1073741824)
		return QString("%1 GB").arg(size / 1073741824.0, 0, 'f', 1);
	else if (size > 1048576)
		return QString("%1 MB").arg(size / 1048576.0, 0, 'f', 1);
	else if (size > 1024)
		return QString("%1 KB").arg(size / 1024.0, 0, 'f', 1);
	else														
		return QString("%1 B").arg(size);
}

void MediaEditModel::startRenamer(QModelIndexList filelist, const QString & name, const QString & ext, 
	const QString & repl, const QString & counter, const QString & cut, int regFlag)
{
	QFileInfoList filenames;

	for (int i = 0; i < filelist.count(); i++) {
		if (filelist.at(i).column() == 0) {
			QFileInfo * item = static_cast<QFileInfo*>(filelist.at(i).internalPointer());
			if (item == nullptr)
				continue;

			if (item->isFile()) {
				filenames << *item;
			}
		}

	}
	processing(filenames, name, ext, repl, counter, cut, regFlag);

	refresh();
}

void MediaEditModel::processing(const QFileInfoList & fileinfo, const QString & name, const QString & ext, 
					const QString & repl, const QString & counter, const QString & cut, int regFlag)
{
	bool isName = false;
	bool isExt = false;
	bool isCounter = false;
	bool isReplace = false;
	bool isCutL = false;
	bool isCutR = false;

	QString templateName = name;

	if (name.contains(PREF::filename)) {
		isName = true;	
		templateName.replace(PREF::filename, "%1");	
	}

	if (name.contains(PREF::counter)) {
		isCounter = true;
		templateName.replace(PREF::counter, "%2");	
	}

	if (ext != PREF::extension) {
		isExt = true;
	}

	QString val1, val2;
	QRegExp rx2("(([\\w*\\D*]+)=([\\w*\\D*]*))");
	int pos2 = 0;
	while ((pos2 = rx2.indexIn(repl, pos2)) != -1) {
		val1 = rx2.cap(2);
		val2 = rx2.cap(3);
		isReplace = true;
		break;
	}

	int midl = 0;
	QRegExp rx3("(\\d+)[<<]+");
	pos2 = 0;
	while ((pos2 = rx3.indexIn(cut, pos2)) != -1) {
		midl = rx3.cap(1).toInt();
		isCutL = true;
		break;
	}

	int midr = 0;
	QRegExp rx4("[>>]+(\\d+)");
	pos2 = 0;
	while ((pos2 = rx4.indexIn(cut, pos2)) != -1) {
		midr = rx4.cap(1).toInt();
		isCutR = true;
		break;
	}

	int counter_start = 0;
	int counter_step = 1;
	int counter_number = 1;
	QRegExp rx5("(\\d+)+[,*\\s*]*(\\d+)*[,*\\s*]*(\\d+)*");
	pos2 = 0;
	while ((pos2 = rx5.indexIn(counter, pos2)) != -1) {
		counter_start = rx5.cap(1).toInt();

		counter_step = rx5.cap(2).toInt();
		if (counter_step == 0)
			counter_step = 1;

		counter_number = rx5.cap(3).toInt();
		isCounter = true;
		break;
	}

	for(auto&& info : fileinfo) {

		QString newName = templateName;

		QString filename = info.completeBaseName();

		if (isCutL) {
			filename.remove(0, midl);
		}

		if (isCutR) {
			filename.remove(filename.length() - midr, midr);
		}

		if (isReplace) {
			filename.replace(val1, val2);
		}

		if (isName) {
			newName = QString(newName).arg(filename);
		}

		if (isCounter)
		{
			newName = QString(newName).arg(counter_start, counter_number, 10, QChar('0'));
			counter_start++;
		}

		if (isExt) {
			newName.append("." + ext);
		}
		else {
			newName.append("." + info.suffix());
		}

		QString dir = info.absolutePath() + "/" + newName;
		QFile::rename(info.absoluteFilePath(), dir);
	}
}

void MediaEditModel::regProcessing(QString & name, int regFlag)
{
	if (regFlag == (int)reg::NO_REG)
		return;

	QString newName = name;

	if (regFlag == (int)reg::ALL_DOWN) {
		name = newName.toLower();
	} 
	else if (regFlag == (int)reg::ALL_UP) {
		name = newName.toUpper();	
	} 
	else if (regFlag == (int)reg::FIRST_UP) {
		QString firstLetter = newName.left(1);
		newName.mid(1);
		newName.prepend(firstLetter.toUpper());
		name = newName;
	} 
	else if (regFlag == (int)reg::ALL_FIRST_UP) {
		QRegExp rx2("(([a-z])-*_*\\d*!*\\s*)");
		int pos2 = 0;
		while ((pos2 = rx2.indexIn(newName, pos2)) != -1) {
			QString val = rx2.cap(2);
			pos2 += rx2.matchedLength(); 
		}	
	}
}