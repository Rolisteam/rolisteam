#ifndef DATA_READER_H
#define DATA_READER_H

#include <QString>

class DataReader
{
    public:
        DataReader(quint32 bufferSize, const char * buffer);
        ~DataReader();

        void reset();

        int left() const;

        quint8 uint8();

        QString string8();

    private:
        char * m_buffer;
        const char * m_pos;
        const char * m_end;
};

#endif
