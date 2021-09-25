#include "MainWindow.hpp"
#include "ui_MainWindow.h"
#include <QFileDialog>
#include <QFile>
#include <QMessageBox>
#include <QDate>
#include <QTime>
#include <QDateTime>
#include <iostream>
#include <QRegularExpression>
#include <QColor>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
}

MainWindow::~MainWindow()
{
	delete ui;
}


void MainWindow::on_loadXmlBtn_clicked()
{
	QString filename = QFileDialog::getOpenFileName(this,tr("Open XML file"),QString(),tr("XML files (*.xml)"));
	if(filename.isEmpty()) return;
	QFile file(filename);
	if(!file.open(QFile::ReadOnly)) {
		QMessageBox::critical(this,tr("Error!"),tr("Couldn't open the file %1!").arg(filename));
		return;
	}
	QFileInfo info(filename);
	QXmlStreamReader xmlstream(&file);
	readXml(xmlstream,info.baseName());
	file.close();
}

void MainWindow::readXml(QXmlStreamReader &xmlReader, QString title, bool setHtml)
{
	if(!xmlReader.readNextStartElement()) {
		QMessageBox::critical(this,tr("Error!"),tr("Malformed XML!"));
		return;
	}
	if(xmlReader.name() != "Log") {
		QMessageBox::critical(this,tr("Error!"),tr("Malformed XML!"));
		return;
	}
	HTML = QString();
	bool isFirstDoc = true;
	convo = xmlToConversation(xmlReader);
	for(auto it = std::begin(convo); it != std::end(convo); ++it) {
		bool isFirstDate = true;
		for(auto zt = std::begin(*it); zt != std::end(*it); ++zt) {
			QDateTime dateTime(it.key(),zt->time);
			if(isFirstDoc) {
				HTML.append(QStringLiteral("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head>"
	"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
	"<title>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</title></head>"
	"<body><h1>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</h1>\n").arg(title).arg(dateTime.toString() ) );
				isFirstDoc = false;
				isFirstDate = false;
			} else if(isFirstDate) {
				HTML.append(QStringLiteral("<h1>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</h1>\n").arg(title).arg(dateTime.toString() ) );
				isFirstDate = false;
			}
			HTML.append(QStringLiteral("<p>(%1) %2: <span style=\"%3\">%4</span></p>\n").arg(zt->time.toString()).arg(zt->sender).arg(zt->style).arg(zt->text)
										);
		}
	}
	if(setHtml) ui->textBrowser->setHtml(HTML);
}

static const QRegularExpression ITALIC("\\[i\\](.+)\\[\\/i\\]");
static const QRegularExpression BOLD("\\[b\\](.+)\\[\\/b\\]");
static const QRegularExpression UNDERSCORE("\\[u\\](.+)\\[\\/u\\]");
static const QRegularExpression FOREGROUND("\\[c\\=([0-9]+)\\](.+)\\[\\/c\\=[0-9]+\\]");
static const QRegularExpression BACKGROUND("\\[a\\=([0-9]+)\\](.+)\\[\\/a\\=[0-9]+\\]");
static const QRegularExpression FOREGROUND1("\\[c\\=([0-9]+)\\](.+)\\[\\/c\\]");
static const QRegularExpression BACKGROUND1("\\[a\\=([0-9]+)\\](.+)\\[\\/a\\]");
static const QColor COLORS[] = {
	QColor(0xFF000000), QColor(0xFF800000), QColor(0xFF008000), QColor(0xFF808000), QColor(0xFF000080), QColor(0xFF800080), QColor(0xFF008080), QColor(0xFFc0c0c0),
	QColor(0xFFc0dcc0), QColor(0xFFa6caf0), QColor(0xFF2a3faa), QColor(0xFF2a3fff), QColor(0xFF2a5f00), QColor(0xFF2a5f55), QColor(0xFF2a5faa), QColor(0xFF2a5fff),
	QColor(0xFF2a7f00), QColor(0xFF2a7f55), QColor(0xFF2a7faa), QColor(0xFF2a7fff), QColor(0xFF2a9f00), QColor(0xFF2a9f55), QColor(0xFF2a9faa), QColor(0xFF2a9fff),
	QColor(0xFF2abf00), QColor(0xFF2abf55), QColor(0xFF2abfaa), QColor(0xFF2abfff), QColor(0xFF2adf00), QColor(0xFF2adf55), QColor(0xFF2adfaa), QColor(0xFF2adfff),
	QColor(0xFF2aff00), QColor(0xFF2aff55), QColor(0xFF2affaa), QColor(0xFF2affff), QColor(0xFF550000), QColor(0xFF550055), QColor(0xFF5500aa), QColor(0xFF5500ff),
	QColor(0xFF551f00), QColor(0xFF551f55), QColor(0xFF551faa), QColor(0xFF551fff), QColor(0xFF553f00), QColor(0xFF553f55), QColor(0xFF553faa), QColor(0xFF553fff),
	QColor(0xFF555f00), QColor(0xFF555f55), QColor(0xFF555faa), QColor(0xFF555fff), QColor(0xFF557f00), QColor(0xFF557f55), QColor(0xFF557faa), QColor(0xFF557fff),
	QColor(0xFF559f00), QColor(0xFF559f55), QColor(0xFF559faa), QColor(0xFF559fff), QColor(0xFF55bf00), QColor(0xFF55bf55), QColor(0xFF55bfaa), QColor(0xFF55bfff),
	QColor(0xFF55df00), QColor(0xFF55df55), QColor(0xFF55dfaa), QColor(0xFF55dfff), QColor(0xFF55ff00), QColor(0xFF55ff55), QColor(0xFF55ffaa), QColor(0xFF55ffff),
	QColor(0xFF7f0000), QColor(0xFF7f0055), QColor(0xFF7f00aa), QColor(0xFF7f00ff), QColor(0xFF7f1f00), QColor(0xFF7f1f55), QColor(0xFF7f1faa), QColor(0xFF7f1fff),
	QColor(0xFF7f3f00), QColor(0xFF7f3f55), QColor(0xFF7f3faa), QColor(0xFF7f3fff), QColor(0xFF7f5f00), QColor(0xFF7f5f55), QColor(0xFF7f5faa), QColor(0xFF7f5fff),
	QColor(0xFF7f7f00), QColor(0xFF7f7f55), QColor(0xFF7f7faa), QColor(0xFF7f7fff), QColor(0xFF7f9f00), QColor(0xFF7f9f55), QColor(0xFF7f9faa), QColor(0xFF7f9fff),
	QColor(0xFF7fbf00), QColor(0xFF7fbf55), QColor(0xFF7fbfaa), QColor(0xFF7fbfff), QColor(0xFF7fdf00), QColor(0xFF7fdf55), QColor(0xFF7fdfaa), QColor(0xFF7fdfff),
	QColor(0xFF7fff00), QColor(0xFF7fff55), QColor(0xFF7fffaa), QColor(0xFF7fffff), QColor(0xFFaa0000), QColor(0xFFaa0055), QColor(0xFFaa00aa), QColor(0xFFaa00ff),
	QColor(0xFFaa1f00), QColor(0xFFaa1f55), QColor(0xFFaa1faa), QColor(0xFFaa1fff), QColor(0xFFaa3f00), QColor(0xFFaa3f55), QColor(0xFFaa3faa), QColor(0xFFaa3fff),
	QColor(0xFFaa5f00), QColor(0xFFaa5f55), QColor(0xFFaa5faa), QColor(0xFFaa5fff), QColor(0xFFaa7f00), QColor(0xFFaa7f55), QColor(0xFFaa7faa), QColor(0xFFaa7fff),
	QColor(0xFFaa9f00), QColor(0xFFaa9f55), QColor(0xFFaa9faa), QColor(0xFFaa9fff), QColor(0xFFaabf00), QColor(0xFFaabf55), QColor(0xFFaabfaa), QColor(0xFFaabfff),
	QColor(0xFFaadf00), QColor(0xFFaadf55), QColor(0xFFaadfaa), QColor(0xFFaadfff), QColor(0xFFaaff00), QColor(0xFFaaff55), QColor(0xFFaaffaa), QColor(0xFFaaffff),
	QColor(0xFFd40000), QColor(0xFFd40055), QColor(0xFFd400aa), QColor(0xFFd400ff), QColor(0xFFd41f00), QColor(0xFFd41f55), QColor(0xFFd41faa), QColor(0xFFd41fff),
	QColor(0xFFd43f00), QColor(0xFFd43f55), QColor(0xFFd43faa), QColor(0xFFd43fff), QColor(0xFFd45f00), QColor(0xFFd45f55), QColor(0xFFd45faa), QColor(0xFFd45fff),
	QColor(0xFFd47f00), QColor(0xFFd47f55), QColor(0xFFd47faa), QColor(0xFFd47fff), QColor(0xFFd49f00), QColor(0xFFd49f55), QColor(0xFFd49faa), QColor(0xFFd49fff),
	QColor(0xFFd4bf00), QColor(0xFFd4bf55), QColor(0xFFd4bfaa), QColor(0xFFd4bfff), QColor(0xFFd4df00), QColor(0xFFd4df55), QColor(0xFFd4dfaa), QColor(0xFFd4dfff),
	QColor(0xFFd4ff00), QColor(0xFFd4ff55), QColor(0xFFd4ffaa), QColor(0xFFd4ffff), QColor(0xFFff0055), QColor(0xFFff00aa), QColor(0xFFff1f00), QColor(0xFFff1f55),
	QColor(0xFFff1faa), QColor(0xFFff1fff), QColor(0xFFff3f00), QColor(0xFFff3f55), QColor(0xFFff3faa), QColor(0xFFff3fff), QColor(0xFFff5f00), QColor(0xFFff5f55),
	QColor(0xFFff5faa), QColor(0xFFff5fff), QColor(0xFFff7f00), QColor(0xFFff7f55), QColor(0xFFff7faa), QColor(0xFFff7fff), QColor(0xFFff9f00), QColor(0xFFff9f55),
	QColor(0xFFff9faa), QColor(0xFFff9fff), QColor(0xFFffbf00), QColor(0xFFffbf55), QColor(0xFFffbfaa), QColor(0xFFffbfff), QColor(0xFFffdf00), QColor(0xFFffdf55),
	QColor(0xFFffdfaa), QColor(0xFFffdfff), QColor(0xFFffff55), QColor(0xFFffffaa), QColor(0xFFccccff), QColor(0xFFffccff), QColor(0xFF33ffff), QColor(0xFF66ffff),
	QColor(0xFF99ffff), QColor(0xFFccffff), QColor(0xFF007f00), QColor(0xFF007f55), QColor(0xFF007faa), QColor(0xFF007fff), QColor(0xFF009f00), QColor(0xFF009f55),
	QColor(0xFF009faa), QColor(0xFF009fff), QColor(0xFF00bf00), QColor(0xFF00bf55), QColor(0xFF00bfaa), QColor(0xFF00bfff), QColor(0xFF00df00), QColor(0xFF00df55),
	QColor(0xFF00dfaa), QColor(0xFF00dfff), QColor(0xFF00ff55), QColor(0xFF00ffaa), QColor(0xFF2a0000), QColor(0xFF2a0055), QColor(0xFF2a00aa), QColor(0xFF2a00ff),
	QColor(0xFF2a1f00), QColor(0xFF2a1f55), QColor(0xFF2a1faa), QColor(0xFF2a1fff), QColor(0xFF2a3f00), QColor(0xFF2a3f55), QColor(0xFFfffbf0), QColor(0xFFa0a0a4),
	QColor(0xFF808080), QColor(0xFFff0000), QColor(0xFF00ff00), QColor(0xFFffff00), QColor(0xFF0000ff), QColor(0xFFff00ff), QColor(0xFF00ffff), QColor(0xFFffffff)
};


QString MainWindow::formatName(const QString &name)
{
	QString newName = name.toHtmlEscaped();
	for(int i = 0; i < 4; ++i) {
	newName = newName.replace(ITALIC, "<em>\\1</em>");
	newName = newName.replace(BOLD, "<span style=\"text-decoration: underline;\">\\1</span>");
	newName = newName.replace(UNDERSCORE, "<strong>\\1</strong>");
	newName = newName.replace(FOREGROUND, "<span style=\"color: \\1;\">\\2</span>");
	newName = newName.replace(BACKGROUND, "<span style=\"background-color: \\1;\">\\2</span>");
	newName = newName.replace(FOREGROUND1, "<span style=\"color: \\1;\">\\2</span>");
	newName = newName.replace(BACKGROUND1, "<span style=\"background-color: \\1;\">\\2</span>");
	}
	for(int i = 0; i < 256; ++i) {
		newName = newName.replace(QStringLiteral("color: %1").arg(i),QStringLiteral("color: %1").arg(COLORS[i].name()));
	}
	return newName;
}

void MainWindow::on_SaveBtn_clicked()
{
	QString savefile = QFileDialog::getSaveFileName(this,tr("Save HTML"),QString(),tr("HTML files (*.html);;Markdown files (*.md)"));
	if(savefile.isEmpty()) return;
	QFile file(savefile);
	if(!file.open(QFile::WriteOnly)) {
		QMessageBox::critical(this,tr("Error!"),tr("Couldn't open the file %1!").arg(savefile));
		return;
	}
	if(savefile.endsWith(QStringLiteral(".md"),Qt::CaseInsensitive)) {
		file.write(ui->textBrowser->toMarkdown().toUtf8());
	} else {
		file.write(HTML.toUtf8());
	}
	file.flush();
	file.close();
}

Conversation MainWindow::xmlToConversation(QXmlStreamReader &xmlReader)
{
	Conversation convo;
	while(xmlReader.readNextStartElement()) {
		QDate date = QDate::fromString(xmlReader.attributes().value("Date").toString(),QStringLiteral("yyyy.MM.dd.") );
		Message msg;
		msg.time = QTime::fromString(xmlReader.attributes().value("Time").toString());
		while(xmlReader.readNextStartElement()) {
			std::cout << xmlReader.name().toString().toStdString() << std::endl;
			if(xmlReader.name() == "From") {
				while(xmlReader.readNextStartElement()) {
					if(xmlReader.name() == "User") {
						msg.sender = formatName(xmlReader.attributes().value("FriendlyName").toString());
						xmlReader.skipCurrentElement();
					}
				}
			} else if(xmlReader.name() == "To") /*xmlReader.skipCurrentElement();*/ while(xmlReader.readNextStartElement()) {
				if(xmlReader.name() == "User") {
					xmlReader.skipCurrentElement();
				}
			}
			else if(xmlReader.name() == "Text") {
				//std::cout << xmlReader.name().toString().toStdString() << std::endl;
				msg.style = xmlReader.attributes().value("Style").toString();
				msg.text = xmlReader.readElementText();
				// xmlReader.skipCurrentElement();
			} else xmlReader.skipCurrentElement();
		}
		convo[date].push_back(msg);
	}
	return convo;
}

void MainWindow::on_saveToFolders_clicked()
{
	QString saveDirectory = QFileDialog::getExistingDirectory(this,tr("Select directory to save into"));
	if(saveDirectory.isEmpty()) return;
	QDir dir(saveDirectory);
	for(auto it = std::begin(convo); it != std::end(convo); ++it) {
		QDateTime dateTime(it.key(),it->at(0).time);
		QString filename = dir.absoluteFilePath(QStringLiteral("%1+0200CEST.html").arg(dateTime.toString(QStringLiteral("yyyy-MM-dd.hhmmss"))));
		QFile file(filename);
		if(!file.open(QFile::WriteOnly)) return;
		file.write(
					QStringLiteral("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head>"
						"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
						"<title>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</title></head>"
						"<body><h1>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</h1>\n").arg("sender").arg(dateTime.toString()).toUtf8()
		);
		for(auto zt = std::begin(*it); zt != std::end(*it); ++zt) {
			file.write(QStringLiteral("<p>(%1) %2: <span style=\"%3\">%4</span></p>\n").arg(zt->time.toString()).arg(zt->sender).arg(zt->style).arg(zt->text).toUtf8());
		}
		file.flush();
		file.setFileTime(dateTime,QFileDevice::FileAccessTime);
		file.setFileTime(dateTime,QFileDevice::FileBirthTime);
		file.setFileTime(dateTime,QFileDevice::FileMetadataChangeTime);
		file.setFileTime(dateTime,QFileDevice::FileModificationTime);
		file.close();
	}
}

void MainWindow::on_massProcessBtn_clicked()
{
	QString loadDirectory = QFileDialog::getExistingDirectory(this,tr("Select directory to load from"));
	if(loadDirectory.isEmpty()) return;
	QString saveDirectory = QFileDialog::getExistingDirectory(this,tr("Select directory to save into"));
	if(saveDirectory.isEmpty()) return;
	QDir dir1(loadDirectory);
	QDir dir2(saveDirectory);
	QDirIterator inputIterator(dir1);
	while(inputIterator.hasNext()) {
		QString inputFilename = inputIterator.next();
		if(inputFilename.endsWith(QStringLiteral(".xml"),Qt::CaseInsensitive)) {
			QFile infile(inputFilename);
			if(!infile.open(QFile::ReadOnly)) {
				QMessageBox::critical(this,tr("Error!"),tr("Couldn't open the file %1!").arg(inputFilename));
				return;
			}
			QFileInfo info(inputFilename);
			QXmlStreamReader xmlReader(&infile);
			bool fail = false;
			if(!xmlReader.readNextStartElement()) {
				QMessageBox::critical(this,tr("Error!"),tr("Malformed XML!"));
				fail = true;
			}
			if(xmlReader.name() != "Log") {
				QMessageBox::critical(this,tr("Error!"),tr("Malformed XML!"));
				fail = true;
			}
			if(!fail) {
				Conversation convo = xmlToConversation(xmlReader);
				infile.close();
				dir2.mkpath(info.baseName());
				QDir dir(dir2.absoluteFilePath(info.baseName()));
				for(auto it = std::begin(convo); it != std::end(convo); ++it) {
					QDateTime dateTime(it.key(),it->at(0).time);
					QString filename = dir.absoluteFilePath(QStringLiteral("%1+0200CEST.html").arg(dateTime.toString(QStringLiteral("yyyy-MM-dd.hhmmss"))));
					QFile file(filename);
					if(!file.open(QFile::WriteOnly)) return;
					file.write(
								QStringLiteral("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01//EN\" \"http://www.w3.org/TR/html4/strict.dtd\"><html><head>"
									"<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">"
									"<title>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</title></head>"
									"<body><h1>Conversation with %1 at %2 on horvathjeno1944@msn.com (msn)</h1>\n").arg(info.baseName()).arg(dateTime.toString()).toUtf8()
					);
					for(auto zt = std::begin(*it); zt != std::end(*it); ++zt) {
						file.write(QStringLiteral("<p>(%1) %2: <span style=\"%3\">%4</span></p>\n").arg(zt->time.toString()).arg(zt->sender).arg(zt->style).arg(zt->text).toUtf8());
					}
					file.flush();
					file.setFileTime(dateTime,QFileDevice::FileAccessTime);
					file.setFileTime(dateTime,QFileDevice::FileBirthTime);
					file.setFileTime(dateTime,QFileDevice::FileMetadataChangeTime);
					file.setFileTime(dateTime,QFileDevice::FileModificationTime);
					file.close();
				}
			}
		}
	}
}
