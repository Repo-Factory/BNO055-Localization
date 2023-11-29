using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class ConnectToBNO055 : MonoBehaviour
{
    public BNO055 sensor; 
    private bool sensorZeroed = false;
    public Vector3 zero = Vector3.zero;

    void Update()
    {
        if (sensorZeroed)
        {
            this.transform.rotation = Quaternion.Euler(sensor.BNO055_Orientation - zero);
        }
        
        if (!sensorZeroed)
        {
            if (sensor.BNO055_Orientation != Vector3.zero)
            {
                zero = sensor.BNO055_Orientation;
                sensorZeroed = true;
            }
        }
    }
}
