objects = make_mul_tree draw_mul_plots draw_single_arm draw_mul_plots_sign_corrected draw_single_arm_sign_corrected

all: $(objects)

$(objects): %:%.C
	g++ -o $@ $< `root-config --cflags --libs`
