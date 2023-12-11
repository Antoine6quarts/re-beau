using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;
using System;

public class camera : MonoBehaviour
{
    //SerialPort data_stream = new SerialPort("/dev/ttyACM0", 115200);
    SerialPort data_stream = new SerialPort("/dev/ttyUSB0", 9600);
    
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
        //Debug.Log(str);

        string[] str_arr = str.Split(',');
        float x_axis_changes, y_axis_changes;
        if (float.TryParse(str_arr[0], out x_axis_changes) && 
            float.TryParse(str_arr[1], out y_axis_changes))
        {
            transform.eulerAngles += new Vector3(x_axis_changes, y_axis_changes, 0);
        }
        /*
        float mouseX = Input.GetAxis("Mouse X");
        float mouseY = Input.GetAxis("Mouse Y");
        transform.eulerAngles += new Vector3(-mouseY * sensitivity, mouseX * sensitivity, 0);
        */
    }
}
