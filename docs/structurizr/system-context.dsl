workspace {

    model {
        user = person "Giraffe User" "A user of the hardware and software suite, controlling the flight." "User"

        group "Giraffe" {
            aprsNetwork = softwaresystem "APRS Network" "" "Existing System"
            giraffeSystem = softwaresystem "Giraffe" "Hardware and software suite for hosting and controlling high altitude balloon flights."
        }

        user -> giraffeSystem "TODO"
        user -> aprsNetwork "Views position reports using aprs.fi"
        giraffeSystem -> aprsNetwork "Broadcasts APRS position reports"
    }

    views {
                systemcontext giraffeSystem "SystemContext" {
            include *
            animation {
                giraffeSystem
                user
                aprsNetwork
            }

            description "The system context diagram for Giraffe."
            properties {
                structurizr.groups false
            }
        }
    }

}