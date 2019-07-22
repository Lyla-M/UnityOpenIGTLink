#include <iomanip>
#include <iostream>
#include <math.h>
#include <cstdlib>

#include "igtlOSUtil.h"
#include "igtlMessageHeader.h"
#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlClientSocket.h"
#include "igtlTrackingDataMessage.h"
#include "igtl_tdata.h"
#include "igtlImageMessage.h"
#include "igtlStatusMessage.h"

#define UnityIGTLinkReceiveAPI _declspec(dllexport)

igtl::ImageMessage::Pointer SendimgMsg;
igtl::ClientSocket::Pointer Unityclientsocket;

extern "C" {
	typedef void(_stdcall* CallbackImage)(void* ImageDataPointer);
	UnityIGTLinkReceiveAPI void _stdcall GetImage(CallbackImage GetImageData)
	//---------------------------------------------------------------------------
	{
		// Create a message buffer to receive header
		igtl::MessageHeader::Pointer headerMsg;
		headerMsg = igtl::MessageHeader::New();
		//------------------------------------------------------------
		// Allocate a time stamp 
		igtl::TimeStamp::Pointer ts;
		ts = igtl::TimeStamp::New();
		//------------------------------------------------------------
		//for (int i = 0; i < 100; i++)
		//{
		// Initialize receive buffer
		headerMsg->InitPack();
		// Receive generic header from the socket
		int r = Unityclientsocket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
		headerMsg->Unpack();
		//------------------------------------------------------------
		// Get time stamp
		igtlUint32 sec;
		igtlUint32 nanosec;
		headerMsg->GetTimeStamp(ts);
		ts->GetTimeStamp(&sec, &nanosec);

		// Check data type and receive data body		
		if (strcmp(headerMsg->GetDeviceType(), "IMAGE") == 0)
		{

			igtl::ImageMessage::Pointer imgMsg;
			imgMsg = igtl::ImageMessage::New();
			imgMsg->SetMessageHeader(headerMsg);
			imgMsg->AllocatePack();
			// Receive transform data from the socket
			Unityclientsocket->Receive(imgMsg->GetPackBodyPointer(), imgMsg->GetPackBodySize());
			// Deserialize the transform data
			// If you want to skip CRC check, call Unpack() without argument.
			int c = imgMsg->Unpack(0);
			if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
			{
				// Retrive the image data
				int   size[3];          // image dimension
				float spacing[3];       // spacing (mm/pixel)
				int   svsize[3];        // sub-volume size
				int   svoffset[3];      // sub-volume offset
				int   scalarType;       // scalar type
				int   endian;           // endian

				scalarType = imgMsg->GetScalarType();
				endian = imgMsg->GetEndian();
				imgMsg->GetDimensions(size);
				imgMsg->GetSpacing(spacing);
				imgMsg->GetSubVolume(svsize, svoffset);
				imgMsg->AllocateScalars();
				imgMsg->GetScalarPointer();
				GetImageData(imgMsg->GetScalarPointer());
			}
		}
		else
		{
			Unityclientsocket->Skip(headerMsg->GetBodySizeToRead(), 0);
		}
	}
	typedef void(_stdcall* CallbackReceiver)(int Cvalue);
	UnityIGTLinkReceiveAPI void _stdcall ReceiverConnect(char*  hostname, int clientport, CallbackReceiver ReceiverConnectionStatus)
	//UnityIGTLinkReceiveAPI void ReceiverConnect(char*  hostname, int clientport, void(*ReceiverConnectionStatus)(int Cvalue))
	{
			
		igtl::ClientSocket::Pointer clientsocket;
		clientsocket = igtl::ClientSocket::New();
		int j = clientsocket->ConnectToServer(hostname, clientport);
		Unityclientsocket = clientsocket;

		if (j == 0)
		{
			ReceiverConnectionStatus(1);
		}
		//------------------------------------------------------------
		// Close connection (The example code never reaches this section ...)
		// clientsocket->CloseSocket();
	}
}

//FILE * pConsole;
//AllocConsole();
//freopen_s(&pConsole, "CONOUT$", "wb", stdout);
//printf("device type not image %s\n", headerMsg->GetDeviceType());
				//FILE * pConsole;
				//AllocConsole();
				//freopen_s(&pConsole, "CONOUT$", "wb", stdout);
				//printf("I'm called \n");
				//printf("ScalarPointerAddress is %p\n", imgMsg->GetScalarPointer());
				//printf("BodyPointerAddress is %p\n", imgMsg->GetPackBodyPointer());
				//printf("GetPackBodySize is %p\n", imgMsg->GetPackBodySize());
				//uint8_t* ptr = (uint8_t*)imgMsg->GetScalarPointer();
				//for (int i = 0; i <= 9000; i++) {
				//	uint8_t val = *(ptr+i);
				//	printf("%d %d \n", i,val);
				//}
			//FILE * pConsole;
			//AllocConsole();
			//freopen_s(&pConsole, "CONOUT$", "wb", stdout);
			//printf("device type Image \n");
			// Create a message buffer to receive transform data