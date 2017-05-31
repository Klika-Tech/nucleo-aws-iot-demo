import config from '../config'

import aws from 'aws-sdk'
const dc = new aws.DynamoDB.DocumentClient()
import http from 'http'

export function handler(event, context, callback) {
    
    var cities = [
            {name: 'Minsk', id: 625144},
            {name: 'New York', id: 5128638},
            {name: 'Los Angeles', id: 5368361},
            {name: 'Seattle', id: 5809844},
            {name: 'Chicago', id: 4887398},
            {name: 'Washington DC', id: 4140963},
            {name: 'Miami', id: 4164138},
            {name: 'San Francisco', id: 5391959},
            {name: 'London', id: 2643743},
            {name: 'Madrid', id: 3117735},
            {name: 'Milan', id: 3173435},
            {name: 'Rome', id: 3169070},
            {name: 'Berlin', id: 2950158},
            {name: 'Prague', id: 3067696},
            {name: 'Paris', id: 2988507},
            {name: 'Moscow', id: 524901}
        ]
        
    var cityIds = cities
        .map(function(d) {return d.id})
        .reduce(function(prev,cur) { return prev + ',' + cur })

    http.get('http://api.openweathermap.org/data/2.5/group?id=' + cityIds + '&units=metric&appid=' + config.owmApiKey,
        function(res) {
            
            var data = ''
            
            res.on('data', function(chunk) {
                data += chunk
            })
            
            res.on('end', function() {
                data = JSON.parse(data)
                
                data = data.list.map(function(d) {
                    return {
                        city: d.id,
                        timestamp: d.dt,
                        temperature: d.main.temp,
                        humidity: d.main.humidity,
                        pressure: d.main.pressure
                    }
                })
                
                data.forEach(function(d) {
                    
                    var params = {
                        TableName: 'nucleo-weather',
                        Item: {
                            city: d.city,
                            timestamp: d.timestamp,
                            temperature: d.temperature,
                            humidity: d.humidity,
                            pressure: d.pressure
                        }
                    }
                    
                    dc.put(params, function(err) {
                      if (err) callback(err)
                    })
                })
            })
            
        }).on('error', function(err) {callback(err)})
}
