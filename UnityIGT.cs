using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Runtime.InteropServices;
using UnityEngine.UI;
using System;
using System.IO;
using System.Net.Sockets;
using AOT;
using System.Threading;
using UnityEngine.SceneManagement;

public class UnityIGT : MonoBehaviour
{

    private delegate void ServerDelegate(int Svalue);
    [DllImport("OpenIGTLink")]
    static extern void ServerConnect(int serverport, double fps, ServerDelegate ServerConnectionStatus);


    private delegate void ReceiverDelegate(int Rvalue);
    [DllImport("OpenIGTLink")]
    static extern void ReceiverConnect(string hostname, int clientport, ReceiverDelegate ReceiverConnectionStatus);

  
    private delegate void PositionDelegate(float PositionX);
    [DllImport("OpenIGTLink")]
    static extern void GetData(float[] orientation, int Osize, float[] position, int Psize, PositionDelegate SentPosition);

  
    private delegate void GetImageDelegate(IntPtr Address);
    [DllImport("OpenIGTLink")]
    static extern void GetImage(GetImageDelegate GetImageData);

    private delegate void CloseSendDelegate();
    [DllImport("OpenIGTLink")]
    static extern void CloseSendSocket();

    private delegate void CloseReceiveDelegate();
    [DllImport("OpenIGTLink")]
    static extern void CloseReceiveSocket();



    public GameObject[] TracketObject;
    float[] ObjectPosition = new float[12];
    float[] ObjectOrientation = new float[16];
    public static GameObject MySprite;
    public static GameObject MySpriteSM;
    public InputField EnterIP;
    string IPAddress;
    bool startR = false;
    bool startS = false;
    static bool Received = false;
    public static byte[] ImageData;
    public static Texture2D ImageTexture;
    Thread ReceiveThread;
    Thread SendThread;

    public static Text Statustext; //disconnect/connect/receiver


    //-----------------For checking rates-----------------------------------//
    //public Text ObjectStatustext; //object found/lost
    //public static Text SentPositionText;
    //public static Text AddressText;
    //public static Text Send;
    //public static Text Receive;
    //public static Text Frame;
    //public static int SD;
    //public static int RC;
    //public static int UD;
    //public float waitTime = 1.0f;
    //public float timer = 0.0f;
    //private bool flag = false; 
    //----------------------------------------------------------------------//

    void Start()
    {
        MySprite = GameObject.FindWithTag("Image");
        MySpriteSM = GameObject.FindWithTag("SmallImage");
        Application.targetFrameRate = 60;
        ImageData = new byte[505120];

        //AddressText = GameObject.FindWithTag("Add").GetComponent<Text>();
        //Receive = GameObject.FindWithTag("Receive").GetComponent<Text>();
        //Frame = GameObject.FindWithTag("Frame").GetComponent<Text>();
        //Send = GameObject.FindWithTag("Send").GetComponent<Text>();
        
    }


    void Update()
    {
        //--------------------- Checking Updating Rate & Receiving Rate----------------//
        //if (flag == true)
        //{            
        //    timer += Time.deltaTime;
        //    UD = UD + 1;
        //    if (timer > waitTime && timer - waitTime <= 0.05f)
        //    {
        //        Frame.text = UD.ToString();
        //        Receive.text = RC.ToString();
        //        Send.text = SD.ToString();
        //        flag = false;
        //    }
        //}
       //------------------------------------------------------------------------------//
        for (int i = 0; i < 4; i++)
        {
            if (TracketObject[i].activeSelf == true)
            {
                //ObjectStatustext.text = "Object Found";
                ObjectPosition[i * 3 + 0] = TracketObject[i].transform.position.x;
                ObjectPosition[i * 3 + 1] = TracketObject[i].transform.position.y;
                ObjectPosition[i * 3 + 2] = TracketObject[i].transform.position.z;
                ObjectOrientation[i * 4 + 0] = TracketObject[i].transform.rotation.x;
                ObjectOrientation[i * 4 + 1] = TracketObject[i].transform.rotation.y;
                ObjectOrientation[i * 4 + 2] = TracketObject[i].transform.rotation.z;
                ObjectOrientation[i * 4 + 3] = TracketObject[i].transform.rotation.w;
            }
            else
            {
                //ObjectStatustext.text = "Object Lost";
                Array.Clear(array: ObjectPosition, index: 0, length: ObjectPosition.Length);
                Array.Clear(array: ObjectOrientation, index: 0, length: ObjectOrientation.Length);
            }
        }       
        Resources.UnloadUnusedAssets();

    }


    public void StartServer()
    {
        ServerConnect(18946, 60, ServerConnectionStatus);
    }
    public void StartClient()
    {
        IPAddress = EnterIP.text;
        ReceiverConnect(IPAddress, 18944, ReceiverConnectionStatus);
    }
    public void AutoSend()
    {
        InvokeRepeating("StartSendData", 0f, 0.03333f);

    }
    public void StartSendData()
    {
        GetData(ObjectOrientation, 4, ObjectPosition, 3, SentPosition);
        //------------------Check Sending Rate ----------------------------------//
        //if (flag == true)
        //{
        //    SD = SD + 1;           
        //}
        //-------------------------------------------------------------------------//
    }

    public void StartReceiveThread()
    {
        startR = true;
        ReceiveThread = new Thread(AutoReceive);
        ReceiveThread.Start();
    }
    public void AutoReceive()
    {
        while (startR == true)
        {
            StartReceivingImage();
        }
    }
    public void StartReceivingImage()
    {
        GetImage(GetImageDataAddress);
    }
    public void InvokeUpdate()
    {
        InvokeRepeating("UpdateTexture", 0f, 0.033333f);     
    }
    public void UpdateTexture()
    {
        if (Received == true)
        {
            ImageTexture = new Texture2D(820, 616, TextureFormat.Alpha8, false);
            ImageTexture.LoadRawTextureData(ImageData);
            ImageTexture.Apply();
            MySprite.GetComponent<RawImage>().texture = ImageTexture;
            MySpriteSM.GetComponent<RawImage>().texture = ImageTexture;
        }
    }


    [MonoPInvokeCallback(typeof(ServerDelegate))]
    private static void ServerConnectionStatus(int Svalue)
    {
        if (Svalue == 1)
        {
            Statustext = GameObject.FindWithTag("Status").GetComponent<Text>();
            Statustext.text = "Connected to Server";
        }
    }
    [MonoPInvokeCallback(typeof(ReceiverDelegate))]
    private static void ReceiverConnectionStatus(int Rvalue)
    {
        if (Rvalue == 1)
        {
            Statustext = GameObject.FindWithTag("Status").GetComponent<Text>();
            Statustext.text = "Receiver Started";
        }
    }
    [MonoPInvokeCallback(typeof(PositionDelegate))]
    private static void SentPosition(float PositionX) //verify plus received position
    {
        //SentPositionText = GameObject.FindWithTag("Sent").GetComponent<Text>();
        //SentPositionText.text = PositionX.ToString();
        //Debug.Log(PositionX);
    }
    [MonoPInvokeCallback(typeof(GetImageDelegate))]
    private static void GetImageDataAddress(IntPtr Address)
    {
        Marshal.Copy(Address, ImageData, 0, 505120);
        Received = true;
        //RC = RC + 1;
    }
    //public void Reset()
    //{
    //    timer = 0.0f;
    //    RC = 0;
    //    SD = 0;
    //    UD = 0;
    //    flag = true;
    //}
    public void LetsGo()
    {
        AutoSend();
        StartClient();
        StartReceiveThread();
        InvokeUpdate();
    }
    public void Restart()
    {
        ReceiveThread.Abort();
        SceneManager.LoadScene("UltrasoundSimulator_Standing_Sep");
    }

    public void Disconnect()
    {
        startR = false;
        CloseReceiveSocket();
        CloseSendSocket();      
        ReceiveThread.Abort();
    }
    private void OnApplicationQuit()
    {
        ReceiveThread.Abort();
    }

}
