using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class camera : MonoBehaviour
{
    SerialPort data_stream = new SerialPort("/dev/ttyACM0", 115200);
    //SerialPort data_stream = new SerialPort("/dev/ttyUSB0", 9600);
    
    /*
    public float sensitivity = 5.0f;
    */

    // Start is called before the first frame update
    void Start()
    {
        Cursor.lockState = CursorLockMode.Locked;
        data_stream.Open();
    }

    // Update is called once per frame
    void Update()
    {
        string str = data_stream.ReadLine().Trim();
        Debug.Log(str);

        string[] str_arr = str.Split(',');
        float roll, pitch, yaw;
        if (float.TryParse(str_arr[0], out roll) && 
            float.TryParse(str_arr[1], out pitch) &&
            float.TryParse(str_arr[2], out yaw))
        {
            transform.eulerAngles = new Vector3(pitch, yaw, roll);
        }
        /*
        float mouseX = Input.GetAxis("Mouse X");
        float mouseY = Input.GetAxis("Mouse Y");
        transform.eulerAngles += new Vector3(-mouseY * sensitivity, mouseX * sensitivity, 0);
        */
    }
}
