#ifndef INTERFACE_CUSTOM_ARG_H
#define INTERFACE_CUSTOM_ARG_H


namespace eventloop
{


class interface_custom_arg
{
protected:
    interface_custom_arg() = default;
    virtual ~interface_custom_arg() = default;

    virtual interface_custom_arg* copy() = 0;
    virtual interface_custom_arg* move() = 0;
};


}

#endif // INTERFACE_CUSTOM_ARG_H
