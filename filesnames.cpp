#include "filesnames.h"

#include <QtConcurrent/QtConcurrent>

FilesNames::FilesNames(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
	ui.setupUi(this);

	model = new MediaEditModel();
	ui.view->setModel(model);
	onFolder("");
	//model->setData("");


	ui.view->readSettings();

	connect(ui.view, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onItemClicked(const QModelIndex &)));
	connect(ui.start, SIGNAL(clicked()), this, SLOT(onStart()));
    connect(ui.close, SIGNAL(clicked()), this, SLOT(close()));

	QFileInfoList driverList = QDir::drives();
	foreach(const QFileInfo & info, driverList) {
		QPushButton * pushButton = new QPushButton(ui.drivers);
		pushButton->setObjectName("push");
		pushButton->setText(info.dir().path());
		pushButton->setMaximumSize(QSize(40, 16777215));
		ui.horizontalLayout_3->addWidget(pushButton);
		connect(pushButton, SIGNAL(clicked()), this, SLOT(onDriver()));
	}
	QSpacerItem * horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);
	ui.horizontalLayout_3->addItem(horizontalSpacer_2);

}

FilesNames::~FilesNames()
{

}

void FilesNames::onItemClicked(const QModelIndex & index)
{
	QFileInfo * info = model->getPropertyItem(index);

	if (info == NULL) {
		return;
	}

	if (info->isDir()) {
		QString path = info->absolutePath() + "/" + info->fileName ();
		onFolder(path);
		//model->setData(path);
	}
}

void FilesNames::onStart()
{
	int regFlag = (int)reg::NO_REG;

	if (ui.rb2->isChecked()) {
		regFlag = (int)reg::ALL_DOWN;
	} else if (ui.rb3->isChecked()) {
		regFlag = (int)reg::ALL_UP;
	} else if (ui.rb4->isChecked()) {
		regFlag = (int)reg::FIRST_UP;
	} else if (ui.rb5->isChecked()) {
		regFlag = (int)reg::ALL_FIRST_UP;
	}

	QtConcurrent::run(this, &FilesNames::startRename);
	//model->startRenamer(ui.view->getSelectedIndexes(), ui.name->text(), ui.ext->text(), ui.replace->text(), ui.counter->text(), ui.cut->text(), regFlag);
}

void FilesNames::onDriver()
{
	QPushButton * pb = dynamic_cast<QPushButton *>(sender());

	if (pb == nullptr) {
		return;
	}
	onFolder(pb->text());
	//model->setData(pb->text());
}

void FilesNames::startRename()
{
	int regFlag = (int)reg::NO_REG;

	if (ui.rb2->isChecked()) {
		regFlag = (int)reg::ALL_DOWN;
	}
	else if (ui.rb3->isChecked()) {
		regFlag = (int)reg::ALL_UP;
	}
	else if (ui.rb4->isChecked()) {
		regFlag = (int)reg::FIRST_UP;
	}
	else if (ui.rb5->isChecked()) {
		regFlag = (int)reg::ALL_FIRST_UP;
	}
	model->startRenamer(ui.view->getSelectedIndexes(), ui.name->text(), ui.ext->text(), ui.replace->text(), ui.counter->text(), ui.cut->text(), regFlag);
}

void FilesNames::onFolder(const QString& foldername)
{
	QtConcurrent::run(model, &MediaEditModel::setData, foldername);
}
