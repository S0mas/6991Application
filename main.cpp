#include <QApplication>
#include <QDebug>
#include "../WizardFramework/Common/include/Device6991/HardwareConnector6991.h"
#include "../WizardFramework/Common/include/ScpiIFVisa.h" 
#include "StevesController.h"

void printOnScreen(QString const& msg) {
	qDebug() << msg;
}

int main(int argc, char *argv[]) {
	//DONT CHANGE
	QApplication a(argc, argv);
	QCoreApplication::setOrganizationName("Bustec");
	QCoreApplication::setOrganizationDomain("Bustec");
	QCoreApplication::setApplicationName("6991_TEST");

	QString connectorName1 = "connector";
	auto connector1 = new HardwareConnector6991(connectorName1);
	auto scpiIF = new ScpiIFVisa;
	Device6991 dev("Device6132", connector1, scpiIF, 256);
	dev.enableScpiCommandsPrints(false);
	Device6132 dev6132(connectorName1, FecIdType::_1, &dev);
	StevesController ctrl(&dev6132);//Gui ctrl
	ctrl.show();
	//DONT CHANGE 

	{
		//TEST CODE START  ----------------------------------------------------------------------------------------------

		//EXAMPLE CODE START -	This code is showing gains and filters for all channels then changes them to selected values and shows again. After that it does measurement and shows results.
		//						To check it out you need to uncomment the code section below till "EXAMPLE CODE END" line.
		//						HINT: Select lines you want to uncomment and click CTRL+U. To comment again click CTRL+K.

		////channel selection
		//std::vector<uint32_t> channels = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
		////show gains and filters
		//printOnScreen("========================GAINS AND FILTERS=======================");
		//for (auto id : channels) {
		//	if (auto gain = dev6132.gain(id); gain)
		//		printOnScreen(QString("Channel: %1 Gain: %2").arg(id).arg(GainType6132::toString(*gain))); //show gains
		//	if (auto filter = dev6132.filter(id); filter)
		//		printOnScreen(QString("Channel: %1 Filter: %2").arg(id).arg(FilterType6132::toString(*filter))); //show filters
		//}

		////setting gain and filter
		//dev6132.setGain(channels, GainType6132::_1);
		//dev6132.setFilter(channels, FilterType6132::_10Hz);
		//printOnScreen("========================AFTER CONF CHANGED=======================");

		////show gains and filters
		//for (auto id : channels) {
		//	if (auto gain = dev6132.gain(id); gain)
		//		printOnScreen(QString("Channel: %1 Gain: %2").arg(id).arg(GainType6132::toString(*gain))); //show gains
		//	if (auto filter = dev6132.filter(id); filter)
		//		printOnScreen(QString("Channel: %1 Filter: %2").arg(id).arg(FilterType6132::toString(*filter))); //show filters
		//}

		////measure and print
		//printOnScreen("========================MEASUREMENT=======================");
		//if (auto data = dev6132.measure(channels); data) {
		//	for (auto singleChannelResult : *data)
		//		if (singleChannelResult)
		//			printOnScreen(QString("%1V").arg(*singleChannelResult));
		//		else
		//			printOnScreen(QString("NOT SELECTED OR ERROR"));
		//	
		//}
		//else
		//	printOnScreen("MEASUREMENT FAILED!");	

		//EXAMPLE CODE END

		//TEST CODE END ----------------------------------------------------------------------------------------------------
	}
	
	return a.exec();
}
