// Dem Fresco-Standard entsprechendes ActionCallback-Template
// written 1997 by Mark-André Hopf
//------------------------------------------------------------

#ifndef Fresco_base_action_h_template
#define Fresco_base_action_h_template

#ifndef Fresco_base_action_h
#define Fresco_base_action_h
#endif

#ifdef ActionCallback
#undef ActionCallback
#endif

template <class T>
class ActionCallback: ixx_extends(ActionImpl)
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

#endif