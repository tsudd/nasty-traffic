create_module(
        TYPE executable
        VERSION 0.0.1
        DESCRIPTION "App with UI for traffic observing"
        DEPENDENCIES
            pcapworker
            Qt6::Widgets
)
