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
	//DONT CHANGE
	
	// CHANGE HERE
	QString ip6991 = "192.168.168.126";		/// SET DEVICE IP HERE
	FecIdType::Type fecId = FecIdType::_1;	/// SET FRONTEND CARD ID (SLOT) _1 OR _2 this test soft doesnt support both cards at same time (I can add second controller if you want)
	// CHANGE HERE

	//DONT CHANGE
	QString connectorName1 = "connector";
	QSettings settings;
	settings.setValue("ip"+ connectorName1, ip6991);
	auto connector1 = new HardwareConnector6991(connectorName1);
	auto scpiIF = new ScpiIFVisa;
	Device6991 dev("Device6132", connector1, scpiIF, 256);
	dev.connect();
	dev.enableScpiCommandsPrints(false);
	Device6132 dev6132(fecId, &dev);	
	StevesController ctrl(&dev6132);//Gui ctrl
	ctrl.show();
	//DONT CHANGE 

	if (dev6132.isValidDev()) {			
		{	
			printOnScreen("6132 Successfully recognized");
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
	}
	else
		printOnScreen("6132 card not recognized or problem with connection!"); // will show up if 6132 will not be recognized on the start of the program
	
	return a.exec();
}
