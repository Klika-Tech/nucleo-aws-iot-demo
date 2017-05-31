import config from '../config'

import aws from 'aws-sdk'
import _ from 'lodash'

const dc = new aws.DynamoDB.DocumentClient()
const iotData = new aws.IotData({endpoint: config.iotEndpoint})

const sensors = {
    temperature: {
        initial: 20,
        maxDelta: .03,
        min: -20,
        max: 40
    },
    humidity: {
        initial: 60,
        maxDelta: .03,
        min: 10,
        max: 99
    },
    accelerometer: {
        initial: [0, 0, 1],
        maxDelta: .5,
        min: -3,
        max: 3
    },
    gyroscope: {
        initial: [0, 0, 0],
        maxDelta: 5,
        min: -100,
        max: 100
    },
    magnetometer: {
        initial: [-.5, -.1, .5],
        maxDelta: .005,
        min: -.6,
        max: .6
    },
    pressure: {
        initial: 1000,
        maxDelta: .1,
        min: 800,
        max: 1060
    }
}


export function handler(event, context, callback) {

    const params = {
        TableName: 'nucleo-metrics',
        KeyConditionExpression: 'metric = :m',
        ExpressionAttributeValues: {
            ':m': 'temperature',
        },
        ScanIndexForward: false,
        Limit: 1
    }

    dc.query(params, (err, data) => {
        
        if (err) {
            callback(err)
            return
        }

        const newSensorValues = _.mapValues(sensors, (sensorConfig, sensor) => {
            return deviateSensor(sensorConfig, data.Items[0].payload[sensor])
        })

        const rnd = Math.random()
        let params
            
        if (event.marker) {
        
            if (!event.markerAlways && rnd > .25) return

            newSensorValues.marker = true
        
            params = {
                topic: 'Nucleo/data',
                payload: JSON.stringify(newSensorValues)
            }
            
        } else
            params = {
                topic: '$aws/things/Nucleo/shadow/update',
                payload: JSON.stringify({
                    state: {
                        reported: newSensorValues
                    }
                })
            }

        iotData.publish(params, err => {
            if (err) callback(err)
        })
    })
}


function deviateSensor(sensorConfig, currentValue) {

    if (currentValue === undefined) currentValue = sensorConfig.initial

    if (_.isArray(currentValue))
        return _.map(currentValue, v => {
            return deviateSensor(sensorConfig, v)
        })

    let newValue = currentValue

    const rnd = Math.random()
    let shift = sensorConfig.maxDelta

    if (currentValue > sensorConfig.max) shift *= 2
    else if (currentValue < sensorConfig.min) shift *= -2

    if (rnd < .3) newValue = newValue + Math.random() * sensorConfig.maxDelta * 2 - shift

    return newValue
}
