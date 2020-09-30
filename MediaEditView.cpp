#include "MediaEditView.h"

#include <QSettings>
#include <QHeaderView>


MediaEditView::MediaEditView(QWidget *parent) : QTreeView(parent) 
{
	readSettings();
}

MediaEditView::~MediaEditView() 
{
	writeSettings();
}

void MediaEditView::writeSettings() 
{
	QSettings settings;
	QString s = objectName();
	settings.beginGroup(s);
		settings.setValue("headerViewState", header()->saveState());
		settings.setValue("headerViewGeom", header()->saveGeometry());
	settings.endGroup();    
}

void MediaEditView::readSettings()
{
	QSettings settings;
	QString s = objectName();
	settings.beginGroup(s);
	header()->restoreState(settings.value("headerViewState").toByteArray());
	header()->restoreGeometry(settings.value("headerViewGeom").toByteArray());
	settings.endGroup();
}

QModelIndexList MediaEditView::getSelectedIndexes() const
{
	return selectedIndexes();
}