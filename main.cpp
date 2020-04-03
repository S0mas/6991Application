#include <QApplication>
#include <QDebug>
#include "../WizardFramework/Common/include/Device6991/gui/TestsView.h"
#include "../WizardFramework/Common/include/Device6991/gui/Controller6991.h"
#include "../WizardFramework/Common/include/Device6991/gui/RegisterController.h"
#include "../WizardFramework/Common/include/Device6991/Device6991.h"
#include "../WizardFramework/Mock/include/HardwareConnectorMock.h"
#include <QTcpServer>

QTcpSocket *clientConnection = nullptr;

void write(QTcpServer* server) {
	QByteArray block;
	QDataStream out(&block, QIODevice::WriteOnly);
	out.setVersion(QDataStream::Qt_5_14);
	out << QString("DUPA");
	if (!clientConnection)
		clientConnection = server->nextPendingConnection();

	clientConnection->write(block);
}

int main(int argc, char *argv[]) {
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("6991_TEST");

	auto server = new QTcpServer(&a);
	server->listen();
	qDebug() << QHostAddress(QHostAddress::LocalHost).toString() << ":" << server->serverPort();

	auto connector1 = new HardwareConnectorMock("MockConnector1");
	QSettings().setValue("ipMockConnector1", "127.0.0.1");
	auto connector2 = new HardwareConnectorMock("MockConnector2");
	QSettings().setValue("ipMockConnector2", "127.0.0.1");

	auto writeButton = new QPushButton("write");
	QObject::connect(writeButton, &QPushButton::clicked, [server]() { write(server); });
	writeButton->show();


	auto device = new AbstractDeviceXX;
	auto device2 = new AbstractDeviceXX;


	auto scpiIF = new ScpiIFMock;

	auto controller6991 = new Controller6991(connector1, scpiIF, server->serverPort());
	auto controller6991_2 = new Controller6991(connector2, scpiIF, server->serverPort());
	auto resgisterController = new RegisterController(device, device2);
	auto testController = new TestsView(device);

	controller6991->show();
	controller6991_2->show();
	//resgisterController->show();
	//testController->show();

	return a.exec();
}
