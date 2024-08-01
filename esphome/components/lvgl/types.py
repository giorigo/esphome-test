from esphome import automation, codegen as cg
from esphome.const import CONF_VALUE
from esphome.cpp_generator import MockObj, MockObjClass

from .defines import CONF_TEXT
from .lvcode import LVGL_COMP, lv_expr


class LvType(cg.MockObjClass):
    def __init__(self, *args, **kwargs):
        parens = kwargs.pop("parents", ())
        super().__init__(*args, parents=parens + (lv_obj_base_t,))
        self.args = kwargs.pop("largs", [(lv_obj_t_ptr, "obj")])
        self.value = kwargs.pop("lvalue", lambda w: w.obj)
        self.has_on_value = kwargs.pop("has_on_value", False)
        self.value_property = None

    def get_arg_type(self):
        return self.args[0][0] if len(self.args) else None


class LvNumber(LvType):
    def __init__(self, *args):
        super().__init__(
            *args,
            largs=[(cg.float_, "x")],
            lvalue=lambda w: w.get_number_value(),
            has_on_value=True,
        )
        self.value_property = CONF_VALUE


uint16_t_ptr = cg.uint16.operator("ptr")
lvgl_ns = cg.esphome_ns.namespace("lvgl")
char_ptr = cg.global_ns.namespace("char").operator("ptr")
void_ptr = cg.void.operator("ptr")
lv_coord_t = cg.global_ns.namespace("lv_coord_t")
LvglComponent = lvgl_ns.class_("LvglComponent", cg.PollingComponent)
LvglComponentPtr = LvglComponent.operator("ptr")
lv_event_code_t = cg.global_ns.namespace("lv_event_code_t")
lv_indev_type_t = cg.global_ns.enum("lv_indev_type_t")
FontEngine = lvgl_ns.class_("FontEngine")
IdleTrigger = lvgl_ns.class_("IdleTrigger", automation.Trigger.template())
ObjUpdateAction = lvgl_ns.class_("ObjUpdateAction", automation.Action)
LvglCondition = lvgl_ns.class_("LvglCondition", automation.Condition)
LvglAction = lvgl_ns.class_("LvglAction", automation.Action)
LvCompound = lvgl_ns.class_("LvCompound")
lv_font_t = cg.global_ns.class_("lv_font_t")
lv_style_t = cg.global_ns.struct("lv_style_t")
# fake parent class for first class widgets and matrix buttons
lv_pseudo_button_t = lvgl_ns.class_("LvPseudoButton")
lv_obj_base_t = cg.global_ns.class_("lv_obj_t", lv_pseudo_button_t)
lv_obj_t_ptr = lv_obj_base_t.operator("ptr")
lv_disp_t_ptr = cg.global_ns.struct("lv_disp_t").operator("ptr")
lv_color_t = cg.global_ns.struct("lv_color_t")
lv_group_t = cg.global_ns.struct("lv_group_t")
LVTouchListener = lvgl_ns.class_("LVTouchListener")
LVEncoderListener = lvgl_ns.class_("LVEncoderListener")
lv_obj_t = LvType("lv_obj_t")
lv_page_t = cg.global_ns.class_("LvPageType", LvCompound)
lv_img_t = LvType("lv_img_t")

# Argument tuple for use in lambdas
LVGL_COMP_ARG = (LvglComponentPtr, LVGL_COMP)


class LvText(LvType):
    def __init__(self, *args, **kwargs):
        super().__init__(
            *args,
            largs=[(cg.std_string, "text")],
            lvalue=lambda w: w.get_property("text"),
            has_on_value=True,
            **kwargs,
        )
        self.value_property = CONF_TEXT


class LvBoolean(LvType):
    def __init__(self, *args, **kwargs):
        super().__init__(
            *args,
            largs=[(cg.bool_, "x")],
            lvalue=lambda w: w.has_state("LV_STATE_CHECKED"),
            has_on_value=True,
            **kwargs,
        )


class LvSelect(LvType):
    def __init__(self, *args, **kwargs):
        super().__init__(
            *args,
            largs=[(cg.int_, "x")],
            lvalue=lambda w: w.get_property("selected"),
            has_on_value=True,
            **kwargs,
        )


class WidgetType:
    """
    Describes a type of Widget, e.g. "bar" or "line"
    """

    def __init__(
        self,
        name: str,
        w_type: LvType,
        parts: tuple,
        schema=None,
        modify_schema=None,
        lv_name=None,
    ):
        """
        :param name: The widget name, e.g. "bar"
        :param w_type: The C type of the widget
        :param parts: What parts this widget supports
        :param schema: The config schema for defining a widget
        :param modify_schema: A schema to update the widget
        """
        self.name = name
        self.lv_name = lv_name or name
        self.w_type = w_type
        self.parts = parts
        if schema is None:
            self.schema = {}
        else:
            self.schema = schema
        if modify_schema is None:
            self.modify_schema = self.schema
        else:
            self.modify_schema = modify_schema
        self.mock_obj = MockObj(f"lv_{self.lv_name}", "_")

    @property
    def animated(self):
        return False

    @property
    def required_component(self):
        return None

    def is_compound(self):
        return self.w_type.inherits_from(LvCompound)

    async def to_code(self, w, config: dict):
        """
        Generate code for a given widget
        :param w: The widget
        :param config: Its configuration
        :return: Generated code as a list of text lines
        """
        raise NotImplementedError(f"No to_code defined for {self.name}")

    def obj_creator(self, parent: MockObjClass, config: dict):
        """
        Create an instance of the widget type
        :param parent: The parent to which it should be attached
        :param config:  Its configuration
        :return: Generated code as a single text line
        """
        return lv_expr.call(f"{self.lv_name}_create", parent)

    def get_uses(self):
        """
        Get a list of other widgets used by this one
        :return:
        """
        return ()
