// Benötige Fresco Templates
// - ActionCallback
// - UpdateCallback
// written 1997,98 by Mark-André Hopf
//------------------------------------------------------------

#ifndef Fresco_base_action_h_template
#define Fresco_base_action_h_template

#include <Fresco/base/action.h>

#ifndef Fresco_base_action_h
#define Fresco_base_action_h
#endif

#ifdef ActionCallback
#undef ActionCallback
#endif

template <class T>
class ActionCallback : ixx_extends(ActionImpl)
{
        typedef void (T::*ActionMemberFunction)();
    public:
        ActionCallback(T* obj, ActionMemberFunction func)
        {
          obj_ = obj;
          func_ = func;
        }
        void execute(_Ix_Env& = _Ix_default_env)
        {
          (obj_->*func_)();
        }

    private:
        T* obj_;
        ActionMemberFunction func_;
};

template <class T>
class UpdateCallback : public ActionCallback<T>
{
		typedef ActionCallback<T> super;
	public:
		UpdateCallback(T* o, ActionMemberFunction f, FrescoObject_ptr a)
			:super(o, f)
		{
			_attached_to = FrescoObject::_duplicate(a);
			_tag = _attached_to->attach(this);
		}
		virtual ~UpdateCallback() {
			_attached_to->detach(_tag);
		}
		void update(_Ix_Env& _env = _Ix_default_env) {
			execute(_env);
		}
	private:
		FrescoObject_var _attached_to;
		XfTag _tag;
};

#endif