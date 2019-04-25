// extra.h - Johan Smet - BSD-3-Clause (see LICENSE)
//
// various extra components

#ifndef LSIM_EXTRA_H
#define LSIM_EXTRA_H

#include "basic.h"

class PullResistor : public CloneComponent<PullResistor> {
public:
    PullResistor(Value pull_to);
    virtual bool is_dirty() const override;
    virtual void process() override;

    Value pull_value() const {return m_pull_to;}
private:
    Value m_pull_to;
};


#endif // LSIM_EXTRA_H