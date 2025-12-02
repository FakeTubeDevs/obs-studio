#include "OBSAbout.hpp"

#include <widgets/OBSBasic.hpp>
#include <utility/RemoteTextThread.hpp>

#include <qt-wrappers.hpp>

#include <json11.hpp>

#include "moc_OBSAbout.cpp"

using namespace json11;

extern bool steam;

static bool isCzechLocale()
{
	QLocale loc;
	return loc.name().startsWith("cs_CZ") || loc.language() == QLocale::Czech;
}

OBSAbout::OBSAbout(QWidget *parent) : QDialog(parent), ui(new Ui::OBSAbout)
{
	setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

	ui->setupUi(this);

	if (isCzechLocale())
		setWindowTitle("O aplikaci");

	ui->name->setText("FakeTube Broadcast Studio");

	QString bitness;
	if (sizeof(void *) == 4)
		bitness = " (32 bit)";
	else if (sizeof(void *) == 8)
		bitness = " (64 bit)";
	QString ver = obs_get_version_string();
	ui->version->setText(ver + bitness);

	// Remove contribution links
	ui->contribute->setVisible(false);
	ui->donate->setVisible(false);
	ui->getInvolved->setVisible(false);

	// Info text
	if (isCzechLocale())
		ui->info->setText("OBS Studio je bezplatný a otevřený software pro nahrávání videa a živé vysílání.");

	// Bottom links translated
	if (isCzechLocale()) {
		ui->about->setText("<a href='#'>O aplikaci</a>");
		ui->authors->setText("<a href='#'>Autoři</a>");
		ui->license->setText("<a href='#'>Licence</a>");
	} else {
		ui->about->setText("<a href='#'>About</a>");
		ui->authors->setText("<a href='#'>Authors</a>");
		ui->license->setText("<a href='#'>License</a>");
	}

	ui->name->setProperty("class", "text-heading");
	ui->version->setProperty("class", "text-large");
	ui->about->setProperty("class", "bg-base");
	ui->authors->setProperty("class", "bg-base");
	ui->license->setProperty("class", "bg-base");
	ui->info->setProperty("class", "");

	connect(ui->about, &ClickableLabel::clicked, this, &OBSAbout::ShowAbout);
	connect(ui->authors, &ClickableLabel::clicked, this, &OBSAbout::ShowAuthors);
	connect(ui->license, &ClickableLabel::clicked, this, &OBSAbout::ShowLicense);

	ShowAbout();
}

void OBSAbout::ShowAbout()
{
	QString desc;
	if (isCzechLocale()) {
		desc = QString::fromUtf8(
			"<h1>FakeTube Broadcast Studio</h1>"
			"<p>FakeTube Broadcast Studio je aplikace pro nahrávání videa a živé vysílání."
			" Umožňuje streamovat na FakeTube a pořizovat lokální záznamy.</p>"
		);
	} else {
		desc = QString::fromUtf8(
			"<h1>FakeTube Broadcast Studio</h1>"
			"<p>FakeTube Broadcast Studio is an application for video recording and live streaming."
			" It allows streaming to FakeTube and local recording.</p>"
		);
	}
	ui->textBrowser->setHtml(desc);
}

void OBSAbout::ShowAuthors()
{
	QString authors;
	if (isCzechLocale()) {
		authors = QString::fromUtf8(
			"Původní autor OBS Studia: Lain Bailey\n"
			"Autor FakeTube Broadcast Studia: Petr Vurm\n"
			"Přispěvatelé: komunita projektu OBS"
		);
	} else {
		authors = QString::fromUtf8(
			"Original OBS Studio author: Lain Bailey\n"
			"FakeTube Broadcast Studio author: Petr Vurm\n"
			"Contributors: OBS project community"
		);
	}
	ui->textBrowser->setPlainText(authors);
}

void OBSAbout::ShowLicense()
{
	QString license;
	if (isCzechLocale()) {
		license = QString::fromUtf8(
			"OBS Studio: Licencováno pod GNU General Public License (viz soubor COPYING).\n\n"
			"FakeTube Broadcast Studio: Všechna práva vyhrazena."
		);
	} else {
		license = QString::fromUtf8(
			"OBS Studio: Licensed under the GNU General Public License (see COPYING).\n\n"
			"FakeTube Broadcast Studio: All rights reserved."
		);
	}
	ui->textBrowser->setPlainText(license);
}
