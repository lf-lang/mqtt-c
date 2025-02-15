# MQTT Library for the C Target of Lingua Franca

This library provides two reactors, `MQTTPublisher` and `MQTTSubscriber` together with a collection
of example programs.

MQTT is a popular publish-and-subscribe framework using in IoT and other applications. Using MQTT, programs publish data on "topics" and subscribe to "topics". A broker, pointed to by a URL, disseminates published topics to all subscribers. The examples here show how a Lingua Franca program can interact with pre-existing MQTT services by either subscribing to their published data or publishing data for their use.

The more advanced examples here show that it possible to use MQTT as a communication medium between separate LF programs, but that it is difficult to get deterministic behavior. This mechanism should be used, in most cases, only if determinism is irrelevant to you.

The [`MQTTPublisher`](src/lib/MQTTPublisher.lf) and [`MQTTSubscriber`](src/lib/MQTTSubscriber.lf) publish and subscribe string data (or arbitrary byte arrays cast to `char*`). These reactors, by default, assign tags at the subscriber based on the physical time of arrival of a subscription notification. However, they also provide a more advanced feature that can, in some circumstances, convey a timestamp from the publisher to the subscriber.  This feature should not be used without a thorough understanding the consequences, which are illustrated in the examples. 

These library reactors are designed to be used with the C target. To compile and run these, you need to install some software (see [Prerequisites](#prerequisites)). 

## Summary of the Examples

* **[MQTTLegacy](src/MQTTLegacy.lf)**: This program illustrates how to interface to a legacy MQTT service that has no connection with Lingua Franca. The `LegacyPublisher` reactor publishes messages every 5s that any legacy service can subscribe to. For example, you can use the command-line `mosquito_sub` utility to subscribe to this service. In addition, the `Listener` reactor can received messages published by any legacy MQTT publisher, such as the command-line `moquito_pub` utility.

* **[MQTTPhysical](src/MQTTPhysical.lf)**: This LF program periodically publishes on a topic and, in a different part of the same program, subscribes to the same topic. The timestamp at the receiving end will be nondeterministically determined from the local physical clock. The difference between the publisher's logical time and the subscriber's logical time is a reasonable measure of the latency through the MQTT broker. This gives behavior similar to an LF [physical connection](https://www.lf-lang.org/docs/handbook/composing-reactors?target=c#physical-connections).

The following examples illustrate more advanced features, particularly the limited ability to convey timestamps from the publisher to the subscriber:

* **[MQTTLogical](src/MQTTLogical.lf)**: This LF program periodically publishes on a topic and, in a different part of the program, subscribes to the same topic. The publisher has `include_timestamp` set to `true`, and the subscriber has `use_physical_time` set to `false`. The program has no other activity, so as long as the time between publishing of events is larger than the total latency through the MQTT broker, the subscriber will see messages one microstep later than the publisher.  By setting a positive `offset` at the subscriber, you can increase the logical time of the received message and get a zero microstep. This gives behavior similar to an LF connection with an `after` delay.

* **[MQTTDistributed](src/MQTTDistributed.lf)**: This is a federated LF program consisting of two unconnected federates that communicate via MQTT. The publisher has `include_timestamp` set to `true`, and the subscriber has `use_physical_time` set to `false`.  Like `MQTTLogical`, there is no other activity in this program, so the subscriber's timestamps will deterministically match those of the publisher. Unlike `MQTTLogical`, however, the microstep will be zero at the subscriber end. Also, the tags will be deterministic at the receiving end regardless of the communication latency because the receiving federate has no reason to advance its logical time unless it receives an MQTT subscription message. You can change the `use_physical_time` parameter of the `MQTTSubscriber` to `true` to get a (nondeterministic) physical connection, similar to `MQTTPhysical`. 

* **[MQTTDistributedActivity](src/MQTTDistributedActivity.lf)**: This is a federated LF program consisting of two unconnected federates that communicate via MQTT, but where the destination reactor has activity that interferes with its ability to use the incoming timestamps from the publisher.  This program will print a warning each time it receives a message. To get rid of the warnings, you can set the `use_physical_time` parameter of the `MQTTSubscriber` to true, and then it will not use the incoming timestamps (except to measure apparent latency).

* **[MQTTDistributedEarliest](src/MQTTDistributedEarliest.lf)**: This is a federated LF program consisting of two unconnected federates that communicate via MQTT, but where the source does not send a timestamp and the destination has `use_physical_time` set to `false`.  The destination assigns the earliest possible tag to the incoming
message, which, in this case, will result in a sequence of tags (0,1), (0,2), (0,3), etc.

## Prerequisites:

### Ubuntu
1. [openSSL](https://github.com/openssl/openssl.git)
```shell
    apt install libssl-dev
```

2. The [Eclipse Paho MQTT C client library](https://github.com/eclipse/paho.mqtt.c).
```shell
    apt install libpaho-mqtt-dev
```

3. A MQTT broker, e.g. Eclipse Mosquitto
```shell
    apt install mosquitto
```

### MacOS
1. [openSSL](https://github.com/openssl/openssl.git). See [https://www.openssl.org](https://www.openssl.org).
2. The [Eclipse Paho MQTT C client library](https://github.com/eclipse/paho.mqtt.c). E.g., the following might work:

```shell
    git clone git@github.com:eclipse/paho.mqtt.c.git
    mkdir /tmp/build.paho ; cd /tmp/build.paho
    cmake -DPAHO_WITH_SSL=TRUE -DPAHO_BUILD_DOCUMENTATION=TRUE \
    		-DPAHO_BUILD_SAMPLES=TRUE ~/paho.mqtt.c
    sudo make install
    export DYLD_LIBRARY_PATH=/usr/local/lib:$DYLD_LIBRARY_PATH
```

The last line could be put in your `~/.bash_profile` file so that you don't have to type each time you run the LF program in a new shell.

To run the compiled code, you need an MQTT broker to be running. For example, the [Mosquitto Eclipse project](https://mosquitto.org/download/) provides one. On a Mac, you can use homebrew to install the Mosquitto broker:
```shell
    brew install mosquitto
```

If you want to start the broker always upon login, you can do this:

```shell
brew services start mosquitto
```

### All Platforms
To start the broker and test it, do this:

1. Start the broker in the background:

    > mosquitto &
    
2. Start a command-line subscriber:

    > mosquitto_sub -v -t 'test/topic'
    
3. In another terminal window, publish a message:

    > mosquitto_pub -t 'test/topic' -m 'Hello World'


## Implementation

The [`MQTTPublisher`](src/lib/MQTTPublisher.lf) and [`MQTTSubscriber`](src/lib/MQTTSubscriber.lf) reactors use the [Paho MQTT Client Library](https://github.com/eclipse/paho.mqtt.c) (see the [documentation](https://www.eclipse.org/paho/files/mqttdoc/MQTTClient/html/_m_q_t_t_client_8h.html)).
