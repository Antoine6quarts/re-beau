using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO.Ports;

public class camera : MonoBehaviour
{
    SerialPort data_stream = new SerialPort("/dev/ttyUSB0", 9600);
    public string receivedString;
    
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
        receivedString = data_stream.ReadLine().Trim();
        Debug.Log(receivedString);

        if (receivedString.Equals("R"))
            transform.eulerAngles += new Vector3(0, 10);
        else if (receivedString.Equals("L"))
            transform.eulerAngles += new Vector3(0, -10);
        else
            Debug.Log("None");
        /*
        float mouseX = Input.GetAxis("Mouse X");
        float mouseY = Input.GetAxis("Mouse Y");
        transform.eulerAngles += new Vector3(-mouseY * sensitivity, mouseX * sensitivity, 0);
        */
    }
}
