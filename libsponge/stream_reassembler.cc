#include "stream_reassembler.hh"

#include <cmath>

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&.../* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity)
    : _output(capacity)
    , _capacity(capacity)
    , _unread(0)
    , _unassemble(0)
    , _unaccept(capacity)
    , _eof(false)
    , _nevereof(false)
    , _buffer{} {}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (data == "") {
        if (eof)
            _eof = true;
        if (_eof && _buffer.empty())
            _output.end_input();
        return;
    }
    size_t _index = index;
    string _data = data;
    if (index < _unassemble) {
        _index = _unassemble;
        if (_data.length() > _unassemble - index)
            _data = data.substr(_unassemble - index);
        else
            _data = "";
    }
    _unaccept = _capacity + _output.bytes_read();
    for (size_t i = 0; i < _data.length(); i++, _index++) {
        if (_index >= _unaccept)
            break;
        bit c = {_index, _data[i]};
        _buffer.insert(c);
    }

    if (eof && (*(_buffer.rbegin())).data != data[data.length() - 1])
        _nevereof = true;
    // 到来的数据正是所期待的seg index
    // char *str = new char[_capacity + 1];
    char *str = new char[_capacity + 1];
    size_t num = 0;
    auto ite = _buffer.begin();
    if (_unassemble == (*ite).idx) {
        size_t pos = _unassemble;
        while (ite != _buffer.end()) {
            if ((*ite).idx == pos) {
                str[num++] = (*ite).data;
                string s(1, (*ite).data);
                _output.write(s);
                pos++;
                ite = _buffer.erase(ite);
            } else
                break;
        }
        str[num] = '\0';
        _unassemble += num;
    }
    if (eof && !_nevereof)
        _eof = true;
    if (_eof && _buffer.empty())
        _output.end_input();

    delete[] str;
}

size_t StreamReassembler::unassembled_bytes() const { return _buffer.size(); }

bool StreamReassembler::empty() const { return _buffer.empty(); }
