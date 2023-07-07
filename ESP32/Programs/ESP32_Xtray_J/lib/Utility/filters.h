#ifndef FILTERS_H
#define FILTERS_H

#include <CircularBuffer.h>
#include <SimpleKalmanFilter.h>
#include <cmath>


namespace Filter
{
    /**
     * A filter with a strong bias (alpha) for the most recent value, and
     * equally distributed weights for the rest of the values.
     */
    template <size_t N = 16>
    class Weighted0
    {
    public:
        // alpha \in [0, 1]
        Weighted0(float alpha) : _alpha{alpha} {}

        float apply(float data)
        {
            if (_buffer.isFull()) {
                _avg -= (_alpha / (_buffer.capacity - 1)) * _buffer.first();
            }
            if (!_buffer.isEmpty()) {
                _avg += (_alpha / (_buffer.capacity - 1) - 0.5) * _buffer.last();
            }
            _avg += _alpha * data;

            _buffer.push(data);
            return _avg;
        }

    private:
        CircularBuffer<float, N> _buffer;
        float _alpha;
        float _avg = 0.0;
    };


    /**
     * A filter with a diminishing weight.
     * `alpha` indicates the initial value.
     * `decay` indicates how much the weight diminishes per data sample.
     *
     * Params should satisfy:
     *
     *      alpha (1 - decay^N) / (1 - decay) <= 1
     */
    template <size_t N = 16>
    class Decaying
    {
    public:
        Decaying(float decay) : _decay{decay}
        {
            // Calculate initial value using decay.
            _alpha = (1 - decay) / (1 - pow(decay, N));
        }

        Decaying(float alpha, float decay) : _alpha{alpha}, _decay{decay} {}


        float apply(float data)
        {
            float wsum = 0;
            float w = _alpha;
            float avg = 0;
            _buffer.push(data);
            int n = _buffer.size();
            for (int i = 0; i < n; i++) {
                avg += w * _buffer[n - i - 1];
                wsum += w;
                w *= _decay;
            }
            if (wsum < 1.0) {
                avg += (1 - wsum) * data; // Weight any excess with the current data.
            }
            return avg;
        }

    private:
        CircularBuffer<float, N> _buffer;
        float _alpha;
        float _decay;
    };

    /**
     * A moving-average filter with equally distributed weights among all values.
     */
    template <size_t N = 16>
    class Average
    {
    public:
        const CircularBuffer<float, N>& buffer() const
        {
            return _buffer;
        }

        float apply(float data)
        {
            if (_buffer.isFull())
                _avg -= _buffer.first();
            _avg += data;
            _buffer.push(data);
            return _avg / _buffer.size();
        }

        void reset()
        {
            _buffer.clear();
            _avg = 0.0;
        }

    private:
        CircularBuffer<float, N> _buffer;
        float _avg = 0.0;
    };

    /**
     * An exponentially-weighted moving-average (EWMA) filter.
     */
    class EWMA
    {
    public:
        // decay \in [0-1] is the smoothing factor. Larger => more emphasis on current value.
        EWMA(float decay) : _decay{decay}, _x{0}, _first{true} {}

        float apply(float data)
        {
            _x = _first ? data : (1 - _decay) * _x + _decay * data;
            return _x;
        }

        void reset()
        {
            _first = true;
            _x = 0;
        }

    private:
        float _decay;
        float _x;
        bool _first;
    };


    template <size_t N = 16>
    class Stable
    {
    public:
        float apply(float data, float epsilon)
        {
            float filteredData = _filter.apply(data);

            // Check if all recent points are stable around a certain point.
            bool isStabilised = isStableAround(filteredData, epsilon);

            bool isFar = (fabs(filteredData - _stableData) > epsilon);
            if (isFar && isStabilised) {
                // Only update the stabilised weight if all recent records are stable.
                _stableData = filteredData;
            }

            return _stableData;
        }

        void reset()
        {
            _stableData = 0.0;
            _filter.reset();
        }

    private:
        float _stableData = 0.0; // Stores the stabilised weight.
        Average<N> _filter;      // Additional filter for stabilising.

        bool isStableAround(float centre, float epsilon) const
        {
            // Check if all recent points are stable around a certain point.
            const auto& buff = _filter.buffer();
            for (int i = 0; i < buff.size(); i++) {
                if (!(fabs(buff[i] - centre) < epsilon)) {
                    return false;
                }
            }
            return true;
        }
    };


    /**
     * Your friendly neighbourhood Kalman Filter.
     */
    class Kalman
    {
        SimpleKalmanFilter _filter;
        float _e_mea, _e_est, _q;

    public:
        // e_mea: Measurement Uncertainty - How much do we expect to our measurement vary
        // e_est: Estimation Uncertainty - Can be initialized with the same value as e_mea since the kalman filter will
        // adjust its value. q: Process Variance - usually a small number between 0.001 and 1 - how fast your
        // measurement moves. Recommended 0.01. Should be tuned to your needs.
        Kalman(float e_mea, float e_est, float q) : _filter{e_mea, e_est, q}, _e_mea{e_mea}, _e_est{e_est}, _q{q} {}

        void reset()
        {
            _filter = SimpleKalmanFilter{_e_mea, _e_est, _q};
        }

        float apply(float data)
        {
            return _filter.updateEstimate(data);
        }
    };


    /**
     * Gathers mean and variance from a stream of data.
     * Implements Knuth's method described here: https://math.stackexchange.com/a/116344/615376.
     */
    class Statistical
    {
    public:
        Statistical() : _first{true}, _m{0}, _v{0} {}

        void reset()
        {
            _first = true;
            _m = _v = _c = 0;
        }

        float apply(float x)
        {
            float m_prev = _m;

            _c++;
            _m = _first ? x : _m + (x - _m) / _c;
            _v = _first ? 0 : _v + (x - _m) * (x - m_prev);

            return _m; // This is kinda arbitrary, but to follow the same prototype as everything else, let's just
                       // return something.
        }

        float mean() const
        {
            return _m;
        }

        float var() const
        {
            return _v;
        }

        size_t count() const
        {
            return _c;
        }

    private:
        bool _first; // Whether the current item is the first sample encountered.
        float _m;    // mean
        float _v;    // variance
        size_t _c;   // count
    };

} // namespace Filter

#endif
