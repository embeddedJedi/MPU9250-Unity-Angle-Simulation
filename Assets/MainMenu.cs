using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using System.IO.Ports;
using TMPro;
using System;
using System.Threading;

public class MainMenu : MonoBehaviour
{
    public SerialPort serialPort;
    public TMP_Dropdown portlist;
    public Button connect;
    public TMP_Text x;
    public TMP_Text y;

    string data;
    string[] value;
    float[] lastRot = { 0, 0, 0 };

    public string[] strdata = new string[3];
    public string[] strData_received = new string[3];
    public float qw, qx, qy, qz;

    public bool setOrigin = false;
    Quaternion rotOffset = Quaternion.identity;

    public GameObject rocket;

    void Start()
    {
        // Seri portları al
        string[] ports = SerialPort.GetPortNames();

        // Seri portları dropdown'a ekle
        portlist.ClearOptions();
        portlist.AddOptions(new List<string>(ports));
        portlist.RefreshShownValue();

        // Bağlan düğmesine tıklandığında
        connect.onClick.AddListener(connected);
    }

    public void connected()
    {
        string selectedPort = portlist.options[portlist.value].text;

        // Background thread for opening the serial port
        Thread serialPortThread = new Thread(() =>
        {
            serialPort = new SerialPort(selectedPort, 9600);
            try
            {
                serialPort.Open();
                Debug.Log("Port " + selectedPort + " opened.");

                // Start reading data
                StartReadingData();
            }
            catch (Exception e)
            {
                Debug.LogError("Error: " + e.Message);
            }
        });
        serialPortThread.Start();
    }

    private void StartReadingData()
    {
        // Start a background thread to read data from the serial port
        Thread readThread = new Thread(() =>
        {
            while (serialPort != null && serialPort.IsOpen)
            {
                try
                {
                    string strReceived = serialPort.ReadLine();
                    lock (strdata)
                    {
                        strdata = strReceived.Split(',');
                    }
                }
                catch (Exception e)
                {
                    Debug.LogError("Error reading from serial port: " + e.Message);
                }
            }
        });
        readThread.Start();
    }

    void Update()
    {
        lock (strdata)
        {
            if (strdata.Length >= 3 && !string.IsNullOrEmpty(strdata[0]) && !string.IsNullOrEmpty(strdata[1]) && !string.IsNullOrEmpty(strdata[2]))
            {
                x.text = strdata[1];
                y.text = strdata[0];

                qy = float.Parse(strdata[0]);
                qx = float.Parse(strdata[1]);
                qz = float.Parse(strdata[2]);

                qw = Quaternion.Euler(qx, qy, qz).w;

                rocket.transform.rotation = Quaternion.Euler(-qx, qz, qy);
            }
        }
    }

    private void OnApplicationQuit()
    {
        // Close the serial port when the application quits
        if (serialPort != null && serialPort.IsOpen)
        {
            serialPort.Close();
        }
    }
}
