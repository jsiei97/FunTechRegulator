#ifndef  __OUTPUT_H
#define  __OUTPUT_H


typedef enum OT
{
    OUTPUT_NONE = 0, ///< No configured output
    OUTPUT_PWM_GPIO  ///< BigPWM on a gpio pin
} OutputType;

class Output
{
    private:
        OutputType type;

        QString name;
        QString in;

    public:
        Output();

        void setName(QString name);
        void setInput(QString out);

        bool putValue(double value);
        void print();
};

#endif  // __OUTPUT_H
