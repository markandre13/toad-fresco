/*
 * vport.cxx
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Fresco/base/fresco.h>
#include <Fresco/base/action.h>
#include <Fresco/base/proxyobs.h>
#include <Fresco/defs/layouts.h>
#include <Fresco/defs/widgets.h>
#include <Fresco/defs/figures.h>
#include "docuview.h"
#include "exampleslib.h"

const Glyph_ptr label(const char* str, FigureKit_ptr figure_kit) {
    FigureStyle_var figstyle = figure_kit->default_style();
    CharString_var char_string = FrescoLib::string_ref(str);
    return figure_kit->label(figstyle, char_string);
}

class ViewportController;
typedef void (ViewportController::*VpFunc)();
class ViewportController {
public:
    ViewportController();

    Viewer_ptr control_gui();
    Viewport_ptr viewport();
    
protected:
    Viewport_var viewport_;
    Glyph_var aligner_;
    Viewer_var control_gui_;

    void create_control_gui();
    Glyph_ptr axis_controller(Axis axis);
    Glyph_ptr home_buttons();
    Glyph_ptr open_file_gui();
    Glyph_ptr hcell(Coord natural);
    void open_file();
    void insert_table();
    void insert_text();
    void insert_image();
    void change_align_field_editors(Alignment x, Alignment y);
    void set_aligner(Glyph_ptr new_aligner);
    void set_target(Glyph_ptr target);
    
    void change_allocation_mode();
    void align_update();
    void usable_update();
    void text_file_update();
    void image_file_update();
    void change_file_to_text();
    void change_file_to_image();
    void change_file_to_hbox();
       
    void begin();
    void end();
    void home();
    
    Button_ptr button(
        const char* str, VpFunc action
    );
    Button_ptr radio_button(
        const char* str, Telltale_ptr group, VpFunc action
    );
    Button_ptr check_box(
        const char* str, VpFunc action
    );

    Glyph_ptr label_and_field(
        const char* str, FieldEditor_ptr fe
    );			      
    
    Telltale_var allocate_natural_[2];
    FrescoObject_var align_callback_[2];
    FieldEditor_var align_fe_[2];
    Alignment init_align_[2];

    FieldEditor_var usable_fe_[2];
    FrescoObject_var usable_callback_[2];
    FullyVisibleConstraint_var fv_constraint_[2];
    Alignment init_usable_[2];
  
    Glyph_var text_, image_, table_;
    char image_str_[256];
    FieldEditor_var text_file_fe_, image_file_fe_;
    FieldSubject_var text_file_fsubj_, image_file_fsubj_;
    FrescoObject_var text_file_callback_, image_file_callback_;
    Button_var text_file_checkbox_, image_file_checkbox_,
	table_checkbox_;
    Telltale_var text_file_checkbox_state_, image_file_checkbox_state_,
	table_checkbox_state_;
    
    LayoutKit_var layout_kit_;
    WidgetKit_var widget_kit_;
    FigureKit_var figure_kit_;
};

declareActionCallback(ViewportController)
implementActionCallback(ViewportController)

ViewportController::ViewportController() {
    layout_kit_ = Fresco_resolve_object(LayoutKit, "layouts");
    widget_kit_ = Fresco_resolve_object(WidgetKit, "widgets");
    figure_kit_ = Fresco_resolve_object(FigureKit, "figures");

    aligner_ = layout_kit_->align(nil, Alignment(0), Alignment(0));
    viewport_ = layout_kit_->create_viewport(aligner_);

    init_align_[X_axis] = Alignment(0.5);
    init_align_[Y_axis] = Alignment(1.0);
    init_usable_[X_axis] = Alignment(0.0);
    init_usable_[Y_axis] = Alignment(0.0);

    text_ = nil; image_ = nil; table_ = nil;
    image_str_[0] = '\0';
    
    text_file_fsubj_ = widget_kit_->field_subject();
    image_file_fsubj_ = widget_kit_->field_subject();
    text_file_fsubj_->replace_all(
        CharString_var(FrescoLib::string_ref("doc/examples/vport.txt"))
    );
    image_file_fsubj_->replace_all(
	CharString_var(FrescoLib::string_ref("doc/examples/image.tif"))
    );
    text_file_checkbox_ = check_box(
        "", &ViewportController::change_file_to_text
    );
    image_file_checkbox_ = check_box(
	"", &ViewportController::change_file_to_image
    );
    table_checkbox_ = check_box(
	"", &ViewportController::change_file_to_hbox
    );
    text_file_checkbox_state_ = text_file_checkbox_->state();
    image_file_checkbox_state_ = image_file_checkbox_->state();
    table_checkbox_state_ = table_checkbox_->state();
    text_file_checkbox_state_->clear(Telltale::chosen);
    image_file_checkbox_state_->clear(Telltale::chosen);
    table_checkbox_state_->set(Telltale::chosen);

    create_control_gui();
    open_file();
}

Viewer_ptr ViewportController::control_gui() {
    return Viewer::_duplicate(control_gui_);
}

void ViewportController::create_control_gui() {
    Coord lr_margin = Coord(12);
    Coord t_margin = Coord(5);
    Coord b_margin = Coord(10);
    
    Glyph_var x_controller = layout_kit_->margin_lrbt(
	Glyph_var(axis_controller(X_axis)),
	lr_margin,lr_margin, b_margin,t_margin
    );
    Glyph_var y_controller = layout_kit_->margin_lrbt(
	Glyph_var(axis_controller(Y_axis)),
	lr_margin,lr_margin, b_margin,t_margin
    );

    Glyph_var upper_part = layout_kit_->hbox();
    upper_part->append(x_controller);
    upper_part->append(Glyph_var(widget_kit_->hseparator()));
    upper_part->append(y_controller);
    
    Glyph_var open_file = layout_kit_->margin_lrbt(
	Glyph_var(open_file_gui()),
	lr_margin,lr_margin, b_margin,t_margin
    );
    Glyph_var home_buttons_gui = layout_kit_->margin_lrbt(
	Glyph_var(home_buttons()),
	lr_margin,lr_margin, b_margin,t_margin
    );
    Glyph_var lower_part = layout_kit_->hbox_align_elements(Alignment(0.5));
    lower_part->append(open_file);
    lower_part->append(Glyph_var(widget_kit_->hseparator()));
    lower_part->append(home_buttons_gui);

    Coord zero = Coord(0);
    Glyph_var vglue3 = layout_kit_->vglue(Coord(3),zero,zero);
    Glyph_var gui = layout_kit_->vbox_align_elements(Alignment(0.5));
    gui->append(Glyph_var(widget_kit_->vseparator()));
    gui->append(upper_part);
    gui->append(Glyph_var(widget_kit_->vseparator()));
    gui->append(vglue3);
    gui->append(lower_part);
    gui->append(Glyph_var(widget_kit_->vseparator()));

    Glyph_var total_hbox = layout_kit_->hbox();
    total_hbox->append(Glyph_var(widget_kit_->hseparator()));
    total_hbox->append(gui);
    total_hbox->append(Glyph_var(widget_kit_->hseparator()));

    Glyph_var final_gui = layout_kit_->margin(total_hbox, Coord(5));
					      
    Glyph_var bg = layout_kit_->back(
	final_gui, Glyph_var(widget_kit_->filler())
    );
    Viewer_var vg = widget_kit_->viewer_group(
	Glyph_var(widget_kit_->outset_frame(bg))
    );
    vg->append_viewer(align_fe_[X_axis]);
    vg->append_viewer(align_fe_[Y_axis]);
    vg->append_viewer(usable_fe_[X_axis]);
    vg->append_viewer(usable_fe_[Y_axis]);
    vg->append_viewer(text_file_fe_);
    vg->append_viewer(image_file_fe_);
    
    control_gui_ = Viewer::_duplicate(vg);
}

void ViewportController::change_allocation_mode() {
    Telltale_var tt;
    tt = allocate_natural_[X_axis];
    Viewport::SpanAllocationMode xmode, ymode;
    xmode = (tt->test(Telltale::chosen)) ? Viewport::natural : Viewport::given;
    tt = allocate_natural_[Y_axis];
    ymode = (tt->test(Telltale::chosen)) ? Viewport::natural : Viewport::given;
    viewport_->set_allocation_mode(xmode, X_axis);
    viewport_->set_allocation_mode(ymode, Y_axis);
    viewport_->need_resize();
    viewport_->need_redraw();
}
void ViewportController::align_update() {
    static bool skip = false; 
    if (skip) return;
    
    char* xstr = align_fe_[X_axis]->get_current();
    char* ystr = align_fe_[Y_axis]->get_current();
    float xalign, yalign;
    if (sscanf(xstr, "%f", &xalign) && sscanf(ystr, "%f", &yalign)){
	Glyph_var data = aligner_->body();
	Glyph_var new_aligner = layout_kit_->align(data, xalign, yalign);
	set_aligner(new_aligner);
	fv_constraint_[X_axis]->align(xalign);
	fv_constraint_[Y_axis]->align(yalign);
	viewport_->need_resize();
	viewport_->need_redraw();

	// For feedback...
	skip = true;
	FieldSubject_var xsubj = align_fe_[X_axis]->field_subject();
	FieldSubject_var ysubj = align_fe_[Y_axis]->field_subject();
	char buf[10];
	sprintf(buf, "%3.2f", xalign);
	xsubj->replace_all(FrescoLib::string_ref(buf));    
	sprintf(buf, "%3.2f", yalign);
	ysubj->replace_all(FrescoLib::string_ref(buf));
	skip = false;
    }
    CORBA::string_free(xstr);
    CORBA::string_free(ystr);
}

void ViewportController::usable_update() {
    static bool skip = false; 
    if (skip) return;
    
    char* xstr = usable_fe_[X_axis]->get_current();
    char* ystr = usable_fe_[Y_axis]->get_current();
    float x_usable, y_usable;
    if (sscanf(xstr, "%f", &x_usable) && sscanf(ystr, "%f", &y_usable)) {
	fv_constraint_[X_axis]->usable(x_usable);
	fv_constraint_[Y_axis]->usable(y_usable);
	
	viewport_->need_resize();
	viewport_->need_redraw();

	// For feedback...
	skip = true;
	FieldSubject_var xsubj = usable_fe_[X_axis]->field_subject();
	FieldSubject_var ysubj = usable_fe_[Y_axis]->field_subject();
	char buf[10];
	sprintf(buf, "%3.2f", x_usable);
	xsubj->replace_all(FrescoLib::string_ref(buf));    
	sprintf(buf, "%3.2f", y_usable);
	ysubj->replace_all(FrescoLib::string_ref(buf));
	skip = false;
    }
    CORBA::string_free(xstr);
    CORBA::string_free(ystr);
}

void ViewportController::set_aligner(Glyph_ptr new_aligner) {
    aligner_ = Glyph::_duplicate(new_aligner);
    viewport_->body(aligner_);
}

void ViewportController::set_target(Glyph_ptr target) {
    aligner_->body(target);
}

void ViewportController::text_file_update() {
    text_file_checkbox_state_->set(Telltale::chosen);
    image_file_checkbox_state_->clear(Telltale::chosen);
    table_checkbox_state_->clear(Telltale::chosen);
    open_file();
}
void ViewportController::image_file_update() {
    image_file_checkbox_state_->set(Telltale::chosen);
    text_file_checkbox_state_->clear(Telltale::chosen);
    table_checkbox_state_->clear(Telltale::chosen);
    open_file();
}
void ViewportController::change_file_to_text() {
    text_file_checkbox_state_->set(Telltale::chosen);
    image_file_fe_->lose_focus(nil, false, false);
    image_file_checkbox_state_->clear(Telltale::chosen);
    table_checkbox_state_->clear(Telltale::chosen);
    open_file();
}
void ViewportController::change_file_to_image() {
    image_file_checkbox_state_->set(Telltale::chosen);
    text_file_fe_->lose_focus(nil, false, false);
    text_file_checkbox_state_->clear(Telltale::chosen);
    table_checkbox_state_->clear(Telltale::chosen);
    open_file();
}
void ViewportController::change_file_to_hbox() {
    table_checkbox_state_->set(Telltale::chosen);
    text_file_fe_->lose_focus(nil, false, false);
    image_file_fe_->lose_focus(nil, false, false);
    text_file_checkbox_state_->clear(Telltale::chosen);
    image_file_checkbox_state_->clear(Telltale::chosen);
    open_file();
}
void ViewportController::open_file() {
    if (text_file_checkbox_state_->test(Telltale::chosen)) {
	insert_text();
    } else if (image_file_checkbox_state_->test(Telltale::chosen)) {
	insert_image();
    } else {
	insert_table();
    }
}

void ViewportController::insert_text() {
    char* filename = text_file_fe_->get_current();
    if (strncmp(filename, "doc/examples", 12) == 0) {
	char* replacement = FrescoLib::fresco_filename(filename);
	CORBA::string_free(filename);
	filename = replacement;
    }
    CharString_var fname = FrescoLib::string_copy(filename);
    CORBA::string_free(filename);

    TextFlow::Kit_var fmt_kit = Fresco_resolve_object(TextFlow::Kit, "text");
    Glyph_var view = new DocumentView(
        fname, layout_kit_, figure_kit_, fmt_kit
    );
    if (is_nil(view)) {
	Display_var display = FrescoLib::current_context()->display_ptr();
	display->ring_bell(.075f);
    } else {
	set_target(Glyph_var(layout_kit_->margin(view, Coord(3))));
	change_align_field_editors(Alignment(0), Alignment(1));
	viewport_->need_resize();
	viewport_->need_redraw();
    }
}

void ViewportController::insert_image() {
    char* filename = image_file_fe_->get_current();
    if (strncmp(filename, "doc/examples", 12) == 0) {
	char* replacement = FrescoLib::fresco_filename(filename);
	CORBA::string_free(filename);
	filename = replacement;
    }
    
    if (strcmp(filename, image_str_) != 0) {
	ImageKit_var image_kit = Fresco_resolve_object(ImageKit, "images");
	Raster_var raster = image_kit->open(filename);
	Display_var display = FrescoLib::current_context()->display_ptr();
	if (is_nil(raster)) {
	    display->ring_bell(.075f);
	} else {
	    Screen_var screen = display->default_screen();
	    Coord dpi = screen->dpi();
	    raster->scale(float(72.0/dpi)); // Have non res-independent raster
	    strcpy(image_str_, filename);
	    image_ = figure_kit_->image(raster);
	}
    }
    CORBA::string_free(filename);

    if (is_not_nil(image_)) {
	set_target(image_);
	change_align_field_editors(Alignment(.5), Alignment(.5));
	viewport_->need_resize();
	viewport_->need_redraw();
    }
}

void ViewportController::insert_table() {
    if (is_nil(table_)) {
    	Grid::Index index;
	index.row = 5; index.col = 5;
	Grid_var grid = layout_kit_->fixed_grid(index);
    
	index.row = 0; index.col = 0;
	grid->replace(
            Glyph_var(layout_kit_->vfixed(
	    Glyph_var(hcell(Coord(60))), Coord(20))),
   	    index
        );
	index.row = 0; index.col = 1;
	grid->replace(Glyph_var(hcell(Coord(85))), index);
	index.row = 0; index.col = 2;
	grid->replace(Glyph_var(hcell(Coord(50))), index);
	index.row = 0; index.col = 3;
	grid->replace(Glyph_var(hcell(Coord(85))), index);
	index.row = 0; index.col = 4;
	grid->replace(Glyph_var(hcell(Coord(60))), index);

	Glyph_var generic = hcell(Coord(10)); 
	index.row = 1; index.col = 0;
	grid->replace(
            Glyph_var(layout_kit_->vfixed(generic, Coord(25))),
	    index
        );
	index.row = 1; index.col = 1;
	grid->replace(generic, index);
	index.row = 1; index.col = 2;
	grid->replace(generic, index);
	index.row = 1; index.col = 3;
	grid->replace(generic, index);
	index.row = 1; index.col = 4;
	grid->replace(generic, index);

	index.row = 2; index.col = 0;
        grid->replace(
            Glyph_var(layout_kit_->vfixed(generic, Coord(25))),
	    index
        );
	index.row = 2; index.col = 1;
	grid->replace(generic, index);
	index.row = 2; index.col = 2;
	grid->replace(generic, index);
	index.row = 2; index.col = 3;
	grid->replace(generic, index);
	index.row = 2; index.col = 4;
	grid->replace(generic, index);

	index.row = 3; index.col = 0;
	grid->replace(
            Glyph_var(layout_kit_->vfixed(generic, Coord(30))),
	    index
        );
	index.row = 3; index.col = 1;
	grid->replace(generic, index);
	index.row = 3; index.col = 2;
	grid->replace(generic, index);
	index.row = 3; index.col = 3;
	grid->replace(generic, index);
	index.row = 3; index.col = 4;
	grid->replace(generic, index);

	index.row = 4; index.col = 0;
	grid->replace(
            Glyph_var(layout_kit_->vfixed(generic, Coord(20))),
	    index
        );
	index.row = 4; index.col = 1;
	grid->replace(generic, index);
	index.row = 4; index.col = 2;
	grid->replace(generic, index);
	index.row = 4; index.col = 3;
	grid->replace(generic, index);
	index.row = 4; index.col = 4;
	grid->replace(generic, index);
	table_ = layout_kit_->margin(grid, Coord(3));
    }
    set_target(table_);
    change_align_field_editors(Alignment(.5), Alignment(.5));
    viewport_->need_resize();
    viewport_->need_redraw();
}

Glyph_ptr ViewportController::hcell(Coord natural) {
    Coord min = Coord(30);
    Glyph::Requisition r;
    GlyphImpl::init_requisition(r);
    GlyphImpl::require(
	r.x, natural, layout_kit_->fil(), natural-min, Coord(0)
    );
    GlyphImpl::require(r.y, Coord(0), Coord(0), Coord(0), Coord(0));
    Glyph_var glue = layout_kit_->glue_requisition(r);
    Glyph_var g = layout_kit_->margin_lrbt(
        Glyph_var(widget_kit_->inset_frame(glue)),
	Coord(1), Coord(1), Coord(1), Coord(1)
    );
    return Glyph::_duplicate(g);
}

Viewport_ptr ViewportController::viewport() {
    return Viewport::_duplicate(viewport_);
}

void ViewportController::change_align_field_editors(Alignment x, Alignment y) {
    FieldSubject_var x_fsubj = align_fe_[X_axis]->field_subject();
    FieldSubject_var y_fsubj = align_fe_[Y_axis]->field_subject();
    char buf[10];
    sprintf(buf, "%3.2f", x);
    x_fsubj->replace_all(FrescoLib::string_copy(buf));
    sprintf(buf, "%3.2f", y);
    y_fsubj->replace_all(FrescoLib::string_copy(buf));
}

void ViewportController::begin() {
    Adjustment_var xadj = viewport_->scroll_adjustment(X_axis);
    Adjustment_var yadj = viewport_->scroll_adjustment(Y_axis);
    xadj->scroll_begin(); yadj->scroll_begin();
}
void ViewportController::end() {
    Adjustment_var xadj = viewport_->scroll_adjustment(X_axis);
    Adjustment_var yadj = viewport_->scroll_adjustment(Y_axis);
    xadj->scroll_end(); yadj->scroll_end();
}

void ViewportController::home() {
    viewport_->need_resize();
    viewport_->need_redraw();
}

Glyph_ptr ViewportController::axis_controller(Axis axis) {
    Glyph_var title;
    if (axis == X_axis) {
	title = label("X axis", figure_kit_);
    } else {
	title = label("Y axis", figure_kit_);
    }
    align_fe_[axis] = widget_kit_->field_editor(nil, 5, 5);
    FieldSubject_var fsubj = align_fe_[axis]->field_subject();
    char buf[10];
    sprintf(buf, "%3.2f", init_align_[axis]);
    fsubj->replace_all(FrescoLib::string_copy(buf));   
    align_callback_[axis] = new ProxyObserver(
	fsubj,
	new ActionCallback(ViewportController)(
	    this, &ViewportController::align_update
        )
    );
    Glyph_var alignment_input = label_and_field(
	"Data alignment", align_fe_[axis]
    );

    usable_fe_[axis] = widget_kit_->field_editor(nil, 5, 5);
    fsubj = usable_fe_[axis]->field_subject();
    sprintf(buf, "%3.2f", init_usable_[axis]);
    fsubj->replace_all(FrescoLib::string_copy(buf));   
    usable_callback_[axis] = new ProxyObserver(
	fsubj,
	new ActionCallback(ViewportController)(
	    this, &ViewportController::usable_update
        )
    );
    Glyph_var usable_input = label_and_field(
	"Usable wt space", usable_fe_[axis]
    );
    fv_constraint_[axis] = layout_kit_->create_fully_visible_constraint(
        init_usable_[axis], init_align_[axis]
    );
    Adjustment_var adj = viewport_->scroll_adjustment(axis);
    adj->constraint(fv_constraint_[axis]);

    Telltale_var tt_group = FrescoLib::telltale_group();
    Button_var allocate_natural= radio_button(
        "Allocate natural",
	tt_group,
	&ViewportController::change_allocation_mode
    );
    Glyph_var allocate_given = radio_button(
        "Allocate given",
	tt_group,
	&ViewportController::change_allocation_mode
    );
    Glyph_var allocate_choice_vbox = layout_kit_->vbox();
    allocate_choice_vbox->append(allocate_natural);
    allocate_choice_vbox->append(allocate_given);
    allocate_natural_[axis] = allocate_natural->state();
    allocate_natural_[axis]->set(Telltale::chosen);

    Glyph_var allocate_choice_box = widget_kit_->inset_frame(
        Glyph_var(layout_kit_->margin_lrbt(
	    allocate_choice_vbox,
	    Coord(10.0), Coord(10.0), Coord(5.0), Coord(5.0)
	))
    );

    Coord zero = Coord(0);
    Glyph_var vglue3 = layout_kit_->vglue(Coord(3),zero,zero);
    Glyph_var vglue7 = layout_kit_->vglue(Coord(7),zero,zero);
    Glyph_var axis_vbox = layout_kit_->vbox_align_elements(Alignment(0.0));
    axis_vbox->append(alignment_input);
    axis_vbox->append(vglue3);
    axis_vbox->append(usable_input);
    axis_vbox->append(vglue7);
    axis_vbox->append(allocate_choice_box);
    
    Glyph_var total_vbox = layout_kit_->vbox_align_elements(Alignment(0.5));
    total_vbox->append(title);
    total_vbox->append(vglue3);
    total_vbox->append(Glyph_var(widget_kit_->vseparator()));
    total_vbox->append(vglue7);
    total_vbox->append(axis_vbox);

    return Glyph::_duplicate(total_vbox);
}		   

Glyph_ptr ViewportController::home_buttons() {
    Glyph_var buttons = layout_kit_->vbox();
    buttons->append(button(
        "begin", &ViewportController::begin
    ));
    buttons->append(button(
        "end", &ViewportController::end
    ));
    buttons->append(button(
        "home", &ViewportController::home
    ));
    return Glyph::_duplicate(buttons);
}

Glyph_ptr ViewportController::open_file_gui() {
    Glyph_var text_label = layout_kit_->halign(
        Glyph_var(label("Text", figure_kit_)),
	Alignment(0)
    );
    Glyph_var image_label = layout_kit_->halign(
	Glyph_var(layout_kit_->vbox_align_elements(Alignment(0.5))),
	Alignment(0)
    );
    Glyph_var table_label = layout_kit_->hbox();
    table_label->append(Glyph_var(layout_kit_->hfil()));
    table_label->append(
        Glyph_var(label("Flexible table", figure_kit_))
    );

    image_label->append(Glyph_var(label("Tiff", figure_kit_)));
    image_label->append(Glyph_var(label("image", figure_kit_)));

    text_file_fe_ = widget_kit_->field_editor(text_file_fsubj_, 20,30);
    image_file_fe_ = widget_kit_->field_editor(image_file_fsubj_, 20,30);

    Glyph_var inset_text_file_fe_ = widget_kit_->inset_frame(text_file_fe_);
    Glyph_var inset_image_file_fe_ = widget_kit_->inset_frame(
	image_file_fe_
    );
    Glyph::Requisition req;
    GlyphImpl::init_requisition(req);
    inset_text_file_fe_->request(req);
 
    Glyph_var wrapped_text_file_fe = layout_kit_->fixed(
	inset_text_file_fe_,
  	Coord(118),
	(req.y.defined)?req.y.natural:Coord(0)
    );
    Glyph_var wrapped_image_file_fe = layout_kit_->fixed(
	inset_image_file_fe_,
  	Coord(118),
	(req.y.defined)?req.y.natural:Coord(0)
    );
    text_file_callback_ = new ProxyObserver(
	text_file_fsubj_,
	new ActionCallback(ViewportController)(
	    this, &ViewportController::text_file_update
        )
    );
    image_file_callback_ = new ProxyObserver(
	image_file_fsubj_,
	new ActionCallback(ViewportController)(
	    this, &ViewportController::image_file_update
        )
    );
    

    Glyph_var vglue8 = layout_kit_->vglue(Coord(8), Coord(0), Coord(0));
    Glyph_var hglue4 = layout_kit_->hglue_fil(Coord(4));
    Grid::Index index;
    index.row = 5; index.col = 5;

    Grid_var grid = layout_kit_->fixed_grid(index);
    index.row = 0; index.col = 2;
    grid->replace(
	 Glyph_var(layout_kit_->valign(table_label, Alignment(0.5))),
	 index
    );
    index.row = 0; index.col = 3;
    grid->replace(hglue4, index);
    index.row = 0; index.col = 4;
    grid->replace(
	 Glyph_var(layout_kit_->valign(table_checkbox_, Alignment(0.5))),
	 index
    );

    index.row = 1; index.col = 0;
    grid->replace(vglue8, index);

    index.row = 2; index.col = 0;
    grid->replace(
        Glyph_var(layout_kit_->valign(image_label, Alignment(0.5))),
        index
    );
    index.row = 2; index.col = 1;
    grid->replace(hglue4, index);
    index.row = 2; index.col = 2;
    grid->replace(
	Glyph_var(layout_kit_->valign(wrapped_image_file_fe, Alignment(0.5))),
	index
    );
    index.row = 2; index.col = 3;
    grid->replace(hglue4, index);
    index.row = 2; index.col = 4;
    grid->replace(
	 Glyph_var(layout_kit_->valign(image_file_checkbox_, Alignment(0.5))),
	 index
    );
   
    index.row = 3; index.col = 0;
    grid->replace(vglue8, index);
    
    index.row = 4; index.col = 0;
    grid->replace(Glyph_var(layout_kit_->valign(text_label, Alignment(0.5))), index);
    index.row = 4; index.col = 1;
    grid->replace(hglue4, index);
    index.row = 4; index.col = 2;
    grid->replace(
        Glyph_var(layout_kit_->valign(wrapped_text_file_fe, Alignment(0.5))),
	index
    );
    index.row = 4; index.col = 3;
    grid->replace(hglue4, index);
    index.row = 4; index.col = 4;
    grid->replace(
	 Glyph_var(layout_kit_->valign(text_file_checkbox_, Alignment(0.5))), index
    );
	
    return Glyph::_duplicate(grid);
}

Button_ptr ViewportController::button(
    const char* str, ActionMemberFunction(ViewportController) action
) {
    Button_ptr button = widget_kit_->push_button(
	Glyph_var(label(str, figure_kit_)),
	Action_var(new ActionCallback(ViewportController)(this, action))
    );
    return button;
}

Button_ptr ViewportController::radio_button(
    const char* str, Telltale_ptr group, ActionMemberFunction(ViewportController) action
) {
    Button_ptr radio_button = widget_kit_->radio_button(
	Glyph_var(label(str, figure_kit_)),
	Action_var(new ActionCallback(ViewportController)(this, action)),
	group
    );
    return radio_button;
}

Button_ptr ViewportController::check_box(
    const char* str, ActionMemberFunction(ViewportController) action
) {
    Button_ptr check_box = widget_kit_->check_box(
	Glyph_var(label(str, figure_kit_)),
	Action_var(new ActionCallback(ViewportController)(this, action))
    );
    return check_box;
}

Glyph_ptr ViewportController::label_and_field(
    const char* str, FieldEditor_ptr fe
) {
    Glyph_var wrapped_fe = layout_kit_->hfixed(
	Glyph_var(widget_kit_->inset_frame(fe)), 
  	Coord(35)
    );
    Glyph_var hbox = layout_kit_->hbox();
    hbox->append(Glyph_var(label(str, figure_kit_)));
    hbox->append(Glyph_var(layout_kit_->hglue_fil(Coord(5))));
    hbox->append(wrapped_fe);
    return Glyph::_duplicate(hbox);
}

// The following registers this example with ExamplesLib naming context. 

static Glyph_ptr example() {
    LayoutKit_var layout_kit = Fresco_resolve_object(LayoutKit, "layouts");
    WidgetKit_var widget_kit = Fresco_resolve_object(WidgetKit, "widgets");
    FigureKit_var figure_kit = Fresco_resolve_object(FigureKit, "figures");
    ViewportController* vpc = new ViewportController;
    Viewport_var viewport = vpc->viewport();
    Adjustment_var xadj = viewport->scroll_adjustment(X_axis);
    Adjustment_var yadj = viewport->scroll_adjustment(Y_axis);
     
    Glyph_var hscrollbar = widget_kit->scroll_bar(X_axis, xadj);
    Glyph_var vscrollbar = widget_kit->scroll_bar(Y_axis, yadj);

    Glyph_var vhaligned_viewport = layout_kit->align(
	viewport, Alignment(0.0), Alignment(1.0)
    );
    Grid::Index index;
    index.row = 2; index.col = 2; 
    Grid_var grid = layout_kit->fixed_grid(index);
    index.row = 0; index.col = 0;
    grid->replace(hscrollbar, index);
    index.row = 1; index.col = 0;
    grid->replace(vhaligned_viewport, index);
    index.row = 1; index.col = 1;
    grid->replace(vscrollbar, index);

    Glyph_var bg = layout_kit->back(
	grid, Glyph_var(widget_kit->filler())
    );    
    Display_var display = FrescoLib::current_context()->display_ptr();
    Screen_var screen = display->default_screen();
    Window_ptr window = screen->application(
        Viewer_var(widget_kit->viewer_group(bg))
    );
    window->map();

    Viewer_ptr gui = vpc->control_gui();
    return gui;
}

static int x = ExamplesLib::bind_example("vport", example);
