using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using UnityEngine;

public class BNO055 : MonoBehaviour
{
    public Vector3 BNO055_Orientation = Vector3.zero;
    private int port = 12121;
    private const int bufferSize = 1024;
    private TcpListener tcpListener;
    private byte[] receiveBuffer = new byte[bufferSize];

    void Start()
    {
        StartListening();
    }

    void Update()
    {
        ReceiveData();
    }

    static float IntToFloat(int num)
    {
        if (num == 0)
        {
            return 0f;
        }
        return (float)num / (float)Math.Pow(10, Math.Floor(Math.Log10(Math.Abs(num)) + 1));
    }

    float fixedToDecimal(int fp)
    {
        int ft = 0;
        for (int i = 0; i < 4; ++i)
        {
            ft += ((fp >> i) & 0x1) * (625 << i);
        }
        float decimalValue = 0;
        decimalValue += ((fp >> 4) + IntToFloat(ft));
        return decimalValue;
    }

    void StartListening()
    {
        try
        {
            tcpListener = new TcpListener(IPAddress.Any, port);

            tcpListener.Start();

            Debug.Log("Socket listener started on port " + port);
        }
        catch (Exception e)
        {
            Debug.LogError("Error starting socket listener: " + e.Message);
        }
    }

    void ReceiveData()
    {
        if (tcpListener == null)
        {
            return;
        }

        if (tcpListener.Pending())
        {
            TcpClient client = tcpListener.AcceptTcpClient();

            NetworkStream stream = client.GetStream();

            int bytesRead = stream.Read(receiveBuffer, 0, bufferSize);

            if (bytesRead > 0)
            {
                short yaw = (short)(receiveBuffer[0] | receiveBuffer[1] << 8);
                short pitch = (short)(receiveBuffer[2] | receiveBuffer[3] << 8);
                short roll = (short)(receiveBuffer[4] | receiveBuffer[5] << 8);
                BNO055_Orientation = new Vector3(360 - fixedToDecimal(pitch), fixedToDecimal(yaw), 360 - fixedToDecimal(roll));
            }

            client.Close();
            stream.Close();
        }
    }

    private void OnDestroy()
    {
        if (tcpListener != null)
        {
            tcpListener.Stop();
            Debug.Log("Socket listener stopped.");
        }
    }
}
