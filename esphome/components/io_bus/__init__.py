import esphome.codegen as cg

CODEOWNERS = ["@clydebarrow"]

io_bus_ns = cg.esphome_ns.namespace("io_bus")
IOBus = io_bus_ns.class_("IOBus")
