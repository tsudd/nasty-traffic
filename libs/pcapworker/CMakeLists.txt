create_module(
        TYPE library
        VERSION 0.0.1
        DESCRIPTION "Interface for traffic analyze"
        EXPORT
            CMAKE YES
            PKGCONF YES
        DEPENDENCIES
            Packet
            wpcap
)
