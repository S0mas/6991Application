#include <QApplication>
#include <QDebug>
#include "../WizardFramework/Common/include/Device6991/HardwareConnector6991.h"
#include "../WizardFramework/Common/include/ScpiIFVisa.h" 
#include "../WizardFramework/Mock/include/HardwareConnectorMock.h"
#include "../WizardFramework/Common/include/Device6991/ScpiIF6991Mock.h"
#include "../WizardFramework/Common/include/gui/Controller6132.h"
#include "../WizardFramework/Common/include/Device6991/gui/Controller6991.h"

Q_DECLARE_METATYPE(StartTestsRequest);
Q_DECLARE_METATYPE(Configuration6991);
Q_DECLARE_METATYPE(FecIdType::Type);

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("6991_TEST");

	qRegisterMetaType<StartTestsRequest>();
	qRegisterMetaType<Configuration6991>();
	qRegisterMetaType<FecIdType::Type>();

	QString connectorName1 = "6991";
	auto connector1 = new HardwareConnector6991(connectorName1);
	auto scpiIF = new ScpiIFVisa;
	//auto connector1 = new HardwareConnectorMock(connectorName1);
	//auto scpiIF = new ScpiIF6991Mock;

	bool dbgMode = false;
	if (argc > 1 && QString(argv[1]) == QString("-dbg"))
		dbgMode = true;

	Controller6991 controller6991(std::make_unique<Device6991>("Device6991", connector1, scpiIF), dbgMode);
	controller6991.show();

	return a.exec();
}
