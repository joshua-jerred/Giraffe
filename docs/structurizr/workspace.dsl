workspace extends ./container.dsl {
    !identifiers hierarchical

    name "Giraffe - A High Altitude Balloon Command and Control System"
    description "Software architecture of Giraffe."

    model {
        !include ./components
    }

    views {
        component giraffeFlightSoftware "FlightSoftware" {
            include *
            animation {
    
            }
    
            description "The component diagram for the flight software."
        }

    component giraffeDataLinkAir "DataLink" {
        include *
        animation {

        }

        description "The component diagram for the data link."
    }
    }
}