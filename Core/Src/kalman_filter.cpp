#include "main.h"
#include <stdio.h>

// MPU6050 specs: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf

// MPU6050 register map: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf

// Coef for converting accel value from mpu (-32768 to +32767) in g force (-2 to +2)
#define MPU_ACCEL_COEF (2.0f / 32767.0f)
// Coef for converting gyro value from mpu (-32768 to +32767) in deg/s force (-250 to +250)
#define MPU_GYRO_COEF  (250.0f / 32767.0f)

// Kalman Filter : https://ahrs.readthedocs.io/en/latest/filters/ekf.html
// Warning : This is an over simplified exemple on only 2 values (rool and roll rate).
// It doesn't have Control Input in Prediction (matrix B and vector u in the link given)
// It doesn't have an Observation Model (matrix H) matrix x and z vectors having the same form

#include "Eigen/Core"
#include "Eigen/Dense"

// System run at 100Hz so delta T is 100Hz => 0.01 seconds of delta
#define DELTA       0.01f
#define RAD_2_DEG   57.2958f
// Coef for converting accel value from mpu (-32768 to +32767) in g force (-4 to +4)
#define MPU_ACCEL_COEF (2.0f / 32767.0f)
// Coef for converting gyro value from mpu (-32768 to +32767) in deg/s (-500 to +500)
#define MPU_GYRO_COEF (250.0f / 32767.0f)

// The system's state
// Don't change this initial value, expect you know the system state at the beginning
Eigen::Vector2f X; // [ roll; roll_rate ]

// The Estimated Covariance Matrix, "confidence" in the prediction
// Don't change this initial value, expect you know the system state at the beginning
Eigen::Matrix2f P = Eigen::Matrix2f::Identity();

// The Model Matrix, how roll and roll_rate are related. Here roll = roll + DELTA * roll_rate, Whereas roll_rate doesn't change
// Change this Matrix regarding the data you want to use, and regarding your device.
const Eigen::Matrix2f F = (Eigen::Matrix2f() << 1.0f, DELTA,
                                                0.0f, 1.0f).finished();

// Those next two matrices define whether the system is more "confident" to the prediction or more to the actual measurement.
// You need to tune these matrices depending on your use case and sensor

// How "not confident"(noise) we are on our prediction
const Eigen::Matrix2f Q = (Eigen::Matrix2f() << 0.05f, 0.0f,
                                                0.0f, 0.1f).finished();                                          

// How "not confident"(noise) we are on our measurement
const Eigen::Matrix2f R = (Eigen::Matrix2f() << 5.0f, 0.0f,
                                                0.0f, 10.0f).finished();

struct MPU6050_data
{
    int8_t accelX_MSB, accelX_LSB;
    int8_t accelY_MSB, accelY_LSB;
    int8_t accelZ_MSB, accelZ_LSB;

    int8_t temp_MSB, temp_LSB;

    int8_t gyroX_MSB, gyroX_LSB;
    int8_t gyroY_MSB, gyroY_LSB;
    int8_t gyroZ_MSB, gyroZ_LSB;
} __attribute__((packed));

// Need to be in an 'extern "C"' block, since this function will be called from a .c file
extern "C"
{
  extern I2C_HandleTypeDef hi2c1;

  void kalman_process()
  {
    // Recover all data from the MPU in one call using our stuct
    // Register 59 to 64 Accelerometer (see register map p29)
    // Register 65 to 66 Accelerometer (see register map 30)
    // Register 67 to 72 Accelerometer (see register map 31)
    struct MPU6050_data data;
    HAL_I2C_Mem_Read(&hi2c1, IMU_ADR_READ, 59, 1, (uint8_t*)&data, sizeof(struct MPU6050_data), HAL_MAX_DELAY);
   
    // Converting our value to g force(Accel) and to deg/sec(Gyro)
    float accelX = MPU_ACCEL_COEF * (((int16_t)data.accelX_MSB << 8) + (int16_t)data.accelX_LSB);
    float accelY = MPU_ACCEL_COEF * (((int16_t)data.accelY_MSB << 8) + (int16_t)data.accelY_LSB);
    float accelZ = MPU_ACCEL_COEF * (((int16_t)data.accelZ_MSB << 8) + (int16_t)data.accelZ_LSB);
    float gyroX = MPU_GYRO_COEF * (((int16_t)data.gyroX_MSB << 8) + (int16_t)data.gyroX_LSB);
    float gyroY = MPU_GYRO_COEF * (((int16_t)data.gyroY_MSB << 8) + (int16_t)data.gyroY_LSB);
    float gyroZ = MPU_GYRO_COEF * (((int16_t)data.gyroZ_MSB << 8) + (int16_t)data.gyroZ_LSB);

    // Print to UART port, using Teleplot syntax
    // printf(">accelX:%f\n", accelX);
    // printf(">accelY:%f\n", accelY);
    // printf(">accelZ:%f\n", accelZ);
    // printf(">gyroX:%f\n", gyroX);
    // printf(">gyroY:%f\n", gyroY);
    // printf(">gyroZ:%f\n", gyroZ);

    // Orientation form Accelerometer
    float accelVector = sqrt((accelX * accelX)
                              + (accelY * accelY)
                              + (accelZ * accelZ));
    float orientation = 0.0f;
    if (abs(accelX) < accelVector)
      orientation = std::asin(accelX / accelVector) * RAD_2_DEG;

    // Measurment Vector z
    Eigen::Vector2f z { orientation, gyroY };

    //---------------------------
    //----- Prediction Step -----
    //---------------------------

    // New state from the previous * the model matrice
    X = F * X;
    // Update how "confident" we arer on prediction.
    // Here we only have one prediction step, but more we have prediction step without correction step, more we get less "confident" in it
    // This depend on our model and on the "noise" of the prediction (matrice Q)
    P = (F * P * F.transpose()) + Q;

    //---------------------------
    //----- Correction Step -----
    //---------------------------
    // "Confidence" of the difference between the predicted measurement and the actual measurement
    const Eigen::Matrix2f S = P + R;
    // Kalman Gain, represent how much the predictions should be corrected
    const Eigen::Matrix2f K = P * S.inverse();
    // Innovation vector it will be used to adjust the state estimate based on the new measurement
    const Eigen::Vector2f v = z - X;
    // We correct the state with the measurement and the caclulated Kalman Gain(K) as well as Innovation vector(v)
    X = X + K * v;
    // We update how confident we are on the prediction
    // Since this is a measurement and not a prediction this matrices will now be closer to the identity matrix 
    P = P - (K * S * K.transpose());

    //printf(">roll:%f\n", X[0]);
    printf(">roll_rate:%f\n", X[1]);
  }
}
