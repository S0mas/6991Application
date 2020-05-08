#include <QApplication>
#include <QDebug>
#include "../WizardFramework/Common/include/Device6991/HardwareConnector6991.h"
#include "../WizardFramework/Common/include/ScpiIFVisa.h" 
#include "../WizardFramework/Mock/include/HardwareConnectorMock.h"
#include "../WizardFramework/Common/include/Device6991/ScpiIF6991Mock.h"
#include "../WizardFramework/Common/include/gui/Controller6132.h"
#include "../WizardFramework/Common/include/Device6991/gui/Controller6991.h"
#include <iostream>
int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("6991_TEST");

	QString connectorName1 = "6991";
	auto connector1 = new HardwareConnector6991(connectorName1);
	auto scpiIF = new ScpiIFVisa;
	//auto connector1 = new HardwareConnectorMock(connectorName1);
	//auto scpiIF = new ScpiIF6991Mock;
	Device6991 dev("Device6991", connector1, scpiIF);
	dev.enableScpiCommandsPrints(false);

	bool dbgMode = false;
	if (argc > 1 && QString(argv[1]) == QString("-dbg"))
		dbgMode = true;

	auto controller6991 = new Controller6991(connector1, scpiIF, dbgMode);
	controller6991->show();

	return a.exec();
}
