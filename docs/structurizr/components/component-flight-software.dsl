
    !ref giraffeFlightSoftware {
        flightRunner = component "Flight Runner"
        configuration = component "Configuration"

        group "Data Module" {
            dataModuleRunnerThread = component "Data Module - Runner Thread"
        }

        group "Server Module" {
            serverModuleRunnerThread = component "Server Module - Runner Thread"
            tcpSocket = component "TCP Socket"
        }

        group "Extension Module" {
            extensionModuleRunnerThread = component "Extension Module - Runner Thread"
        }

        group "Shared Data" {
            dataStreams = component "Data Streams"
            dataFrames = component "Data Frames"
            dataBlocks = component "Data Blocks"
        }

        group "Console Module" {
            consoleModuleRunnerThread = component "Console Module - Runner Thread"
            # TODO: Add console components (user)
        }

        flightRunner -> configuration "Loads the configuration file at startup."
        flightRunner -> dataModuleRunnerThread "Starts the data module."
        flightRunner -> serverModuleRunnerThread "Starts the server module."

        dataModuleRunnerThread -> dataStreams "Reads data from the data streams."
        dataModuleRunnerThread -> dataFrames "Publishes data to the data frames."
        dataModuleRunnerThread -> dataBlocks "Publishes data to the data blocks."

        serverModuleRunnerThread -> tcpSocket "Listens for incoming TCP connections and handles them."
        serverModuleRunnerThread -> dataBlocks "Reads data from the data blocks."
        serverModuleRunnerThread -> dataFrames "Reads data from the data frames."
        serverModuleRunnerThread -> dataStreams "Publishes stats to the data streams."

        consoleModuleRunnerThread -> dataBlocks "Reads data from the data blocks."
        consoleModuleRunnerThread -> dataFrames "Reads data from the data frames."
    }