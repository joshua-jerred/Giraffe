workspace extends ./system-context.dsl {

    model {
        !ref giraffeSystem {
            group "Giraffe Ground Station" {
                giraffeGroundStation  = container "Giraffe Ground Station"  "Ground Station for Giraffe" "" ""
                giraffeWebClient      = container "Giraffe Web Client"      "React Based Web Client" "JS/React" "Web Browser"
                giraffeDataLinkGround = container "Giraffe Data Link (GGS)" "" "" ""
                influxGround          = container "InfluxDB - Ground"       "" "" "Database"

            }

            group "Giraffe Flight System" {
                giraffeFlightSoftware = container "Giraffe Flight Software" "" "" ""
                giraffeDataLinkAir = container "Giraffe Data Link (GFS)" "" "" ""
                giraffeFlightWatchdog = container "Giraffe Flight Watchdog" "" "" ""
                influxAir = container "InfluxDB - Flight" "" "" "Database"
            }
        }

        # relationships to/from containers
        user                  -> giraffeWebClient "TODO"
        giraffeWebClient      -> giraffeGroundStation "TODO"
        giraffeGroundStation  -> giraffeDataLinkGround "TODO"
        giraffeGroundStation  -> influxGround "TODO"
        giraffeDataLinkGround -> influxGround "TODO"

        giraffeDataLinkAir    -> giraffeDataLinkGround "TODO"
        giraffeDataLinkGround -> giraffeDataLinkAir "TODO"
        giraffeGroundStation  -> giraffeFlightSoftware "TCP Socket"

        giraffeDataLinkAir    -> aprsNetwork "TODO"
        giraffeFlightSoftware -> giraffeDataLinkAir "TODO"
        giraffeFlightWatchdog -> giraffeFlightSoftware "TODO"
        giraffeFlightSoftware -> influxAir "TODO"
        
    }

    views {

        container giraffeSystem "Containers" {
            include *
            animation {
                user
                giraffeWebClient
                giraffeDataLinkGround
                aprsNetwork
            }

            description "The container diagram for the Internet Banking System."
        }


        styles {
            element "Person" {
                color #ffffff
                fontSize 22
                shape Person
            }
            element "User" {
                background #08427b
            }
            element "Software System" {
                background #1168bd
                color #ffffff
            }
            element "Existing System" {
                background #999999
                color #ffffff
            }
            element "Container" {
                background #438dd5
                color #ffffff
            }
            element "Web Browser" {
                shape WebBrowser
            }
            element "Database" {
                shape Cylinder
            }
            element "Component" {
                background #85bbf0
                color #000000
            }
            element "Failover" {
                opacity 25
            }
        }
    }

}