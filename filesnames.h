#ifndef FILESNAMES_H
#define FILESNAMES_H

#include <QMainWindow>
#include "ui_filesnames.h"

#include "MediaEditModel.h"

class FilesNames : public QMainWindow
{
	Q_OBJECT

public:
	FilesNames(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~FilesNames();

public slots:
	void onItemClicked(const QModelIndex & index);
	void onStart();
	void onDriver();

	void startRename();
	void onFolder(const QString & foldername);

private:
	Ui::FilesNamesClass ui;
	MediaEditModel * model;
};

#endif // FILESNAMES_H
