#include <iostream>
#include <vector>
#include <common.hpp>

using std::ostream;

ostream &operator<<(ostream &os, const std::vector<float> &dt)
{
    for (int i = 0; i < dt.size(); i++)
    {
        os << dt[i];

        if (i < dt.size() - 1)
            os << ", ";
    }
    return os;
}

int main(int argc, char *argv[])
{
    Common::NeuralNetwork l_nn{2, 4};

    l_nn.setRandomWeights(123);

    Common::NNSize l_nnSize = l_nn.size();

    std::vector<float> l_firstLayer{1.0f, 4.0f, 3.0f, 2.0f};
    l_nn.setActivation([](float p_value)
                       { return std::max(0.0f, p_value); });

    try
    {
        l_nn.work(l_firstLayer);
    }
    catch (std::exception &const e)
    {
        std::cerr << e.what() << std::endl;
    }

    for (int i = 0; i < l_nnSize.layerAmount; i++)
    {
        std::cout << "-------------------------- LAYER " << i << " --------------------------" << std::endl;
        auto l_layer = l_nn.at(i);

        for (auto const &l_node : l_layer)
        {
            std::cout << l_node.value << " [" << l_node.weights << "]" << std::endl;
        }
    }

    return 0;
}