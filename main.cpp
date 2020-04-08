#include <QApplication>
#include <QDebug>
#include "../WizardFramework/Common/include/Device6991/gui/TestsView.h"
#include "../WizardFramework/Common/include/Device6991/gui/Controller6991.h"
#include "../WizardFramework/Common/include/Device6991/gui/RegisterController.h"
#include "../WizardFramework/Common/include/Device6991/Device6991.h"

#define MOCK // needs to be commented in real app

#ifdef MOCK
#include "../WizardFramework/Mock/include/HardwareConnectorMock.h"
#include "../WizardFramework/Common/include/Device6991/ScpiIF6991Mock.h"
#elif
#include "../WizardFramework/Common/include/Device6991/HardwareConnector6991.h"
#include "../WizardFramework/Common/include/ScpiIFVisa.h" 
#endif

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("6991_TEST");
	QString ip6991 = "127.0.0.1"; // 6991 device ip - in mock version it should be localhost
	QString connectorName1 = "connector1";
	QString connectorName2 = "connector2";
#ifdef MOCK
	auto connector1 = new HardwareConnectorMock(connectorName1); 
	auto connector2 = new HardwareConnectorMock(connectorName2);
	auto scpiIF = new ScpiIF6991Mock;
#elif
	auto connector2 = new HardwareConnector6991(connectorName1);
	auto connector2 = new HardwareConnector6991(connectorName2);
	auto scpiIF = new ScpiIFVisa;
#endif
	QSettings().setValue(connectorName1, ip6991);
	QSettings().setValue(connectorName2, ip6991);

	Device6991 dev("Device6111", connector1, scpiIF, 256);

	auto controller6991 = new Controller6991(connector1, scpiIF, 1);
	auto controller6991_2 = new Controller6991(connector2, scpiIF, 2);
	auto resgisterControllerFrontend = new RegisterControllerFrontend(connector1, scpiIF);
	auto resgisterController6991 = new RegisterController6991(connector1, scpiIF);
	auto testController = new TestsView(connector1, scpiIF);

	controller6991->show();
	controller6991_2->show();
	resgisterControllerFrontend->show();
	resgisterController6991->show();
	testController->show();

	return a.exec();
}
