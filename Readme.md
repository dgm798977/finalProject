# 🧪 Final project - Racket strokes detection using neural network model on Arduino Nano BLE 33 - Redes de sensores electrónicos

## Introduction

The purpose of this final project is to obtain a device that could differenciate between different racket strokes by previously doing some training procedures and model obtaining using  **Edge Impulse** platform. 

Project will be divided into data acquisition, modelling/training and final implementation.

## Data acquisition

Data for model training will be obtained using the same Arduino Nano BLE 33. We will need to attach the Arduino BLE into our racket in a way that could detect the movements of the racket while doing different strokes.

Data collected will need to be previously pre-classified and sent to Edge Impulse as a way to properly train the data.

![Edge Impulse](https://i.ibb.co/HDsVyprp/image.png "Data obtained using the data aquisition assembly")

### Methodology

### Population sample

Data acquisition has been done by repeating movements in the air for each stroke we want to detect (due to the lack of time).

### Electronic functioning of data acquisition

As indicated before, Arduino Nano 33 BLE will be the board used as the sensing device of the different strokes done. Data captured will be from accelerometer (3-axis). In order to not have the board connected to a computer which would be used as the *data uploader* to the **Edge Impulse** environment, data will be sent to an ESP32 dev Board using the BLE capability of both devices, being the ESP32 the one to connect to the **API Ingestion** of Edge Impulse. Using  the API Ingestion procedure makes it easy to upload bunch of data directly to the platform.
This way, we will have the data ready to be processed on the project on following steps.

### Electronic functioning of recognition mode

In the final implementation which would result in recognising different strokes that could be made, we will only need the Arduino Nano 33 BLE. This board will be attached to the racket and will recognize the movements done by reading the accelerometer. Results of predictions can be read by connecting your device to the BLE broadcast.

![BLE broadcast](https://i.ibb.co/tp5zGRMG/image.png "BLE broadcast on movement done")

## Modelling & Training

After having obtained a considerable amount of samples of our different strokes to identify we will have to dig into the process of modelling and training of our neural network deployment. As we don't have many different strokes to classify, the structure of the Neural Network will be kept simple, keeping the standard layers Edge Impulse suggests. 

Training will also be done by implementing the procedures Edge Impulse platform suggests.

![Confusion matrix](https://i.ibb.co/NgbXkS13/image.png  "Results of training on Edge Impulse")

# Final implementation

The result of a correct training will be tested with the same procedure. To keep the experiment simple, different strokes will have to be identified by the board and results of identification will be published via BLE so you can read it using any BLE app as nRF Connect by Nordic

![Architecture Diagram](https://i.ibb.co/JR2d2mvY/image.png "Configuration on racket to obtain movements")