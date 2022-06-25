// Hadamar_codes.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#define FILE_IN_OUT
//#define CONSOLE_IN_OUT

#ifdef FILE_IN_OUT
std::ofstream output("output.txt");
std::ifstream input("input.txt");
#endif // FILE_IN_OUT
#ifdef CONSOLE_IN_OUT
std::ostream& output = std::cout;
std::istream& input = std::cin;
#endif // CONSOLE_IN_OUT


void matrix_dot_vector(const std::vector<std::vector<int64_t>>& matr, 
    const std::vector<int64_t>& vect, std::vector<int64_t>& res_vect) {
    res_vect.assign(matr.size(), 0);

    for (int i = 0; i < matr.size(); i++)
    {
        int row_col_sum = 0;
        for (int j = 0; j < vect.size(); j++)
        {
            row_col_sum += matr[i][j] * vect[j];
        }
        res_vect[i] = row_col_sum;
    }
}

void buildHadamarMatrix(std::vector<std::vector<int64_t>>& HadamardMatrix, 
    std::vector<std::vector<int64_t>>& negHadamardMatrix, int block_size) {
    for (int i = 0; i < block_size - 1; i++)
    {
        //  [1;2] square
        for (int j = 0; j < HadamardMatrix.size(); j++) {
            int cur_row_size = HadamardMatrix[j].size();
            for (int k = 0; k < cur_row_size; k++)
                HadamardMatrix[j].push_back(HadamardMatrix[j][k]);
        }

        //  [2;1], [2;2]*(-1) squares
        int cur_column_size = HadamardMatrix.size();
        for (int j = 0; j < cur_column_size; j++)
            HadamardMatrix.push_back(HadamardMatrix[j]);

        //  [2;2] square
        for (int j = HadamardMatrix.size() / 2; j < HadamardMatrix.size(); j++)
            for (int k = HadamardMatrix[j].size() / 2; k < HadamardMatrix[j].size(); k++)
                HadamardMatrix[j][k] *= -1;
    }
    //-H matrix
    negHadamardMatrix.resize(HadamardMatrix.size());
    for (int j = 0; j < HadamardMatrix.size(); j++)
        for (int k = 0; k < HadamardMatrix[j].size(); k++)
            negHadamardMatrix[j].push_back(HadamardMatrix[j][k] * (-1));

}

void dataEncoding(const std::vector<std::vector<int64_t>>& HadamardMatrix,
    const std::vector<std::vector<int64_t>>& negHadamardMatrix,
    const std::string& data, int block_size, std::string& result_string) {

    for (size_t i = 0; i < data.size() / block_size; i++)
    {
        std::vector<int64_t> coding_row;
        int row_number = 0;
        unsigned int bin_counter = 1;

        for (int j = block_size * (i + 1) - 1; j > block_size * i; j--)
            row_number += (bin_counter <<= 1) / 2 * (data[j] - '0');

        coding_row = ((data[block_size * i] - '0') ? negHadamardMatrix[row_number] : HadamardMatrix[row_number]);

        for (int j = 0; j < coding_row.size(); j++)
            coding_row[j] = (coding_row[j] + 1) / 2;


        for (int j = 0; j < coding_row.size(); j++)
            result_string.push_back(coding_row[j] + '0');
    }
}

void dataDecoding(const std::vector<std::vector<int64_t>>& HadamardMatrix,
    const std::vector<std::vector<int64_t>>& negHadamardMatrix,
    const std::string& encoded_data, int block_size, std::string& result_string) {
    
    for (size_t i = 0; i < encoded_data.size() / block_size; i++)
    {
        std::vector<int64_t> data_block;
        
        for (int j = block_size * i; j < block_size * (i + 1); j++)
            data_block.push_back(-1 + 2*(encoded_data[j] - '0'));

        std::vector<int64_t> F_vector;

        matrix_dot_vector(HadamardMatrix, data_block, F_vector);

        output << "Block №" << i+1 << " F-vector: ";
        for (int j = 0; j < F_vector.size(); j++)
            output << F_vector[j] << ' ';
        output << '\n';

        int max_abs_pos = 0, max_abs = 0;
        for (int j = 0; j < F_vector.size(); j++)
        {
            int cur_val_abs = ((F_vector[j] >= 0) ? F_vector[j] : (-1) * F_vector[j]);
            max_abs = ((F_vector[max_abs_pos] >= 0) ? F_vector[max_abs_pos] : (-1) * F_vector[max_abs_pos]);
            if (cur_val_abs > max_abs)
            {
                max_abs = cur_val_abs;
                max_abs_pos = j;
            }    
        }

        
        int decoded_block_size = 0;
        unsigned int bin_counter = F_vector.size();
        while (bin_counter!=0)
        {
            bin_counter >>= 1;
            decoded_block_size++;
        }
        

        result_string.push_back((F_vector[max_abs_pos] > 0) ? '0' : '1');
        
        for (int j = 0; j < decoded_block_size-1; j++)
            result_string.push_back((max_abs_pos >> (decoded_block_size - 1 - 1 - j)) % 2 + '0');
    }
}


int main()
{
    int data_length;
    int block_size;

    std::vector<std::vector<int64_t>> HadamardMatrix = { {1} };
    std::vector<std::vector<int64_t>> negHadamardMatrix;

    std::string input_data;

    output << "Original data:\n";
    input >> input_data;
    output << input_data << '\n';

    data_length = input_data.length();

    output << "\nBlock size:\n";
    input >> block_size;
    output << block_size << '\n';

    for (int i = 0; i < (block_size - (data_length % block_size)) % block_size; i++)
    {
        input_data = "0" + input_data;
    }

    output << "\nPrepared data:\n";
    output << input_data << '\n';

    buildHadamarMatrix(HadamardMatrix, negHadamardMatrix, block_size);

    output << "\nHadamard matrix:\n";
    for (int j = 0; j < HadamardMatrix.size(); j++) {
        for (int k = 0; k < HadamardMatrix[j].size(); k++) {
            if (HadamardMatrix[j][k] == 1)
                output << " 1 ";
            else
                output << " - ";
        }

        output << "\n";
    }

    output << "\n\n";

    output << "Negative Hadamard matrix:\n";
    for (int j = 0; j < negHadamardMatrix.size(); j++) {
        for (int k = 0; k < negHadamardMatrix[j].size(); k++) {
            if (negHadamardMatrix[j][k] == 1)
                output << " 1 ";
            else
                output << " - ";
        }

        output << "\n";
    }

    std::string encoded_data = "";

    dataEncoding(HadamardMatrix, negHadamardMatrix, input_data, block_size, encoded_data);

    output << "\nEncoded data:\n" << encoded_data << '\n';

    int mistake_number;
    output << "\nMistake position:\n";
    input >> mistake_number;
    output << mistake_number << '\n';
    encoded_data[mistake_number] = ((encoded_data[mistake_number] == '1') ? ('0') : ('1'));

    output << "\nData with mistake\n";
    for (int j = 0; j < encoded_data.size(); j++)
        if (j == mistake_number)
            output << '!';
        else
            output << ' ';

    output << "\n" << encoded_data << "\n";
    output << "\n-----------Decoding-----------\n\n";

    std::string decoded_data;
    dataDecoding(HadamardMatrix, negHadamardMatrix, encoded_data, HadamardMatrix.size(), decoded_data);

    output << "\nOriginal data: " << decoded_data << "\nRestored data: " << input_data;
}


