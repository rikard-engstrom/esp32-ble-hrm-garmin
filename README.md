# Heart rate service that a garmin watch can connect to

It was easy to create a heart rate service and to connect to it with [nrfConnect](https://apps.apple.com/se/app/nrf-connect/id1054362403)

I started with implementing the bare minimum. 
Since most parts of the hrm specifications are optional, the first step was to do these parts.

```
 Service: Heart Rate UUID: 180D
    Characteristic: Heart Rate Measurement UUID: 2A37
        Descriptor: Client Characteristic Configuration UUID: 2902
```

Examin the service with nrfConnect it looks like it was working fine with working notifications.

My Garmin FR935 could discover the service but failed to connect to it.

I tried different things with advertising and descriptors.
I eventually started to explor what a Wahoo TICKR exposed and implemented the services one by one.
Nothing changed until i added body sensor location. With that in place the watch could use the service.

I haven't done any tests by removing things again to se what is needed but i think the only thing needed is.

```
 Service: Heart Rate UUID: 180D
    Characteristic: Heart Rate Measurement UUID: 2A37
        Descriptor: Client Characteristic Configuration UUID: 2902
    Characteristic: Body Sensor Location UUID: 2A38
```