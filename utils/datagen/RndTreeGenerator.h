#ifndef INFORF_RNDTREEGENERATOR_H
#define INFORF_RNDTREEGENERATOR_H
template <typename T> class cli_option_t {
  public:
    cli_option_t() : _value{}, _set{false} {}
    cli_option_t(const T &v) : _value{v}, _set{false} {}

    T get() {
        if (!_set) { throw std::runtime_error{"Option was not set"}; }
        return _value;
    }

    bool set() { return _set; }

    cli_option_t<T> &operator=(const T &v) {
        _value = v;
        _set = true;
        return *this;
    }

    operator T() const {
        if (!_set) { throw std::runtime_error{"Value was not set"}; }
        return _value;
    }

  private:
    T _value;
    bool _set;
};
class RndTreeGenerator {
  public:
    struct cli_options_t {
        cli_option_t<size_t> trees;
        cli_option_t<size_t> taxa;
        cli_option_t<unsigned> seed;
    };

	static std::string create_random_trees(cli_options_t &opts);
};
#endif // INFORF_RNDTREEGENERATOR_H
