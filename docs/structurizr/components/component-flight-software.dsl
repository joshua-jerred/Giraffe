model {
    !ref giraffeFlightSoftware {
        flightRunner = component "Flight Runner"
        configuration = component "Configuration"

        group "Data Module" {
            dataModuleRunnerThread = component "Data Module - Runner Thread"
            dataStreams = component "Data Streams"
            dataFrames = component "Data Frames"
            dataBlocks = component "Data Blocks"
        }

        group "Server Module" {
            serverModuleRunnerThread = component "Server Module - Runner Thread"
            tcpSocket = component "TCP Socket"
        }

        group "Extension Module" {
            extensionModuleRunnerThread = component "Extension Module - Runner Thread"
        }

        flightRunner -> configuration "Loads the configuration file at startup."
        flightRunner -> dataModuleRunnerThread "Starts the data module."
        flightRunner -> serverModuleRunnerThread "Starts the server module."
    }
}

views {

    component giraffeFlightSoftware "Components" {
        include *
        animation {

        }

        description "The component diagram for the flight software."
    }
}
