#include "ttddbg_tracer_choosers.hh"
#include "ttddbg_tracer.hh"
#include "ttddbg_debugger.hh"

#include <name.hpp>
#include <dbg.hpp>
#include <kernwin.hpp>

namespace ttddbg {
	// Function trace list
	TracerTraceChooser::TracerTraceChooser()
		: chooser_t(CH_CAN_DEL | CH_KEEP | CH_CAN_INS, 2, new int[2]{20, 0}, new char* [2] {"Offset", "Function name"}, "Traced functions")
	{
		this->icon = 128;
	}

	size_t TracerTraceChooser::get_count() const {
		return FunctionTracer::getInstance()->countTraced();
	}

	void TracerTraceChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		func_t* func = FunctionTracer::getInstance()->funcAt(n);

		qstring fname;
		get_func_name(&fname, func->start_ea);
		qstring demangled;
		demangled = demangle_name(fname.c_str(), 0);

		if (demangled.size() > 0) {
			fname = demangled;
		}

		out->at(0).sprnt("0x%X", func->start_ea);
		out->at(1).sprnt(fname.c_str());
	}

	chooser_t::cbret_t TracerTraceChooser::del(size_t n) {
		FunctionTracer::getInstance()->removeTrace(n);

		return ALL_CHANGED;
	}

	chooser_t::cbret_t TracerTraceChooser::ins(ssize_t n) {
		func_t *func = choose_func("Choose function to trace", 0);
		if (func == nullptr) {
			return NOTHING_CHANGED;
		}

		FunctionTracer::getInstance()->traceFunction(func);

		return ALL_CHANGED;
	}

	//////////////////////////////////////////////////////////////////////////////////////
	TracerEventChooser::TracerEventChooser()
		: chooser_t(CH_KEEP | CH_CAN_DEL, 6, new int[6] {20, 0, 50, 50, 50, 50}, new char* [6] {"Position", "Function name", "Arg1", "Arg2", "Arg3", "Arg4"}, "Tracing events")
	{
		this->icon = 73;
	}

	size_t TracerEventChooser::get_count() const {
		return FunctionTracer::getInstance()->countEvents();
	}

	void TracerEventChooser::get_row(qstrvec_t* out, int* out_icon, chooser_item_attrs_t* out_attrs, size_t n) const {
		FunctionInvocation ev = FunctionTracer::getInstance()->eventAt(n);
		qstring fname;
		get_func_name(&fname, ev.func->start_ea);

		qstring demangled;
		demangled = demangle_name(fname.c_str(), 0);

		if (demangled.size() > 0) {
			fname = demangled;
		}

		out->at(0).sprnt("0x%X:0x%X", ev.pos.Major, ev.pos.Minor);
		out->at(1).sprnt("%s", fname.c_str());

		if (ev.is_return) {
			*out_icon = 57;
		}
		else {
			*out_icon = 56;
		}
		
		
		for (int i = 0; i < min(ev.args.size(), 4); i++) {
			out->at(i + 2).sprnt("%s", ev.args[i].c_str());
		}

	}

	ea_t TracerEventChooser::get_ea(size_t n) const {
		FunctionInvocation ev = FunctionTracer::getInstance()->eventAt(n);
		
		static_cast<ttddbg::Debugger*>(dbg)->getManager().setNextPosition(ev.pos);
		continue_process();

		return BADADDR;
	}

	chooser_t::cbret_t TracerEventChooser::del(size_t n) {
		FunctionTracer::getInstance()->removeEvent(n);
		return ALL_CHANGED;
	}
}