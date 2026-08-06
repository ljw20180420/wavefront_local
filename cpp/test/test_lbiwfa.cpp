#include "../src/lbiwfa.hpp"
#include "../src/ini.hpp"
#include "../src/score.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>
#include <chrono>

std::vector<int32_t> get_python_wavefront_score(std::string file_name)
{
    std::ifstream fin(file_name);
    if (!fin.is_open())
    {
        std::exit(1);
    }
    std::string line;
    std::getline(fin, line);

    std::vector<int32_t> s_ts;
    while (std::getline(fin, line))
    {
        std::stringstream ss(line);
        std::string s_t;
        std::getline(ss, s_t, ',');
        s_ts.push_back(std::stoi(s_t));
    }

    return s_ts;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::exit(1);
    }
    auto cfg = read_ini(argv[1]);

    double total_time = 0;
    for (const auto score_schema_name_ : cfg["score_schema"])
    {
        std::string score_schema_name = std::get<std::string>(score_schema_name_);
        for (const auto end_up_ : cfg["end_up"])
        {
            int32_t end_up = std::get<int>(end_up_);
            for (auto mode_ : cfg["mode"])
            {
                std::string mode = std::get<std::string>(mode_);
                char c_f, c_r;
                if (mode == "global")
                {
                    c_f = 'M';
                    c_r = 'M';
                }
                else
                {
                    // mode == "local"
                    c_f = 'L';
                    c_r = 'L';
                }
                for (auto length_ : cfg["length"])
                {
                    int32_t length = std::get<int>(length_);
                    for (auto err_ : cfg["err"])
                    {
                        std::string err = std::get<std::string>(err_);

                        std::string python_file = "test/py/align/pylwfa/" + score_schema_name + '/' + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";
                        std::vector<int32_t> s_ts = get_python_wavefront_score(python_file);

                        std::string test_file = "test/data/" + std::string(mode) + '/' + std::to_string(length) + '_' + err + '_' + std::to_string(end_up) + ".csv";

                        std::cerr << test_file << ": " << total_time << " ms\n";
                        std::ifstream fin(test_file);
                        if (!fin.is_open())
                        {
                            std::cerr << "Error: Could not open file.\n";
                            std::exit(1);
                        }

                        std::vector<int32_t>::iterator it = s_ts.begin();
                        std::string line;
                        std::getline(fin, line);
                        while (std::getline(fin, line))
                        {
                            std::string t, q;
                            std::stringstream ss(line);
                            std::getline(ss, t, ',');
                            std::getline(ss, q);

                            std::string aln;
                            aln.resize(t.size() + q.size());

                            auto start = std::chrono::high_resolution_clock::now();

                            char *aln_t;
                            int32_t s_t;
                            if (score_schema_name == "bowtie")
                            {
                                BiFront<'M', bowtie_wfa.p, 'F'> FrM_f;
                                BiFront<'I', bowtie_wfa.p, 'F'> FrI_f;
                                BiFront<'D', bowtie_wfa.p, 'F'> FrD_f;
                                BiFront<'M', bowtie_wfa.p, 'R'> FrM_r;
                                BiFront<'I', bowtie_wfa.p, 'R'> FrI_r;
                                BiFront<'D', bowtie_wfa.p, 'R'> FrD_r;
                                FrontXM FrXM;
                                Front FrI, FrD;
                                aln_t = lbiwfa<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j, bowtie_wfa.p>(q, t, c_f, c_r, FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln.data());
                                aln.resize(aln_t - aln.data());
                                s_t = eval_score<bowtie_wfa.x, bowtie_wfa.o, bowtie_wfa.e, bowtie_wfa.j>(aln, q, t);
                            }
                            else
                            {
                                // score_schema_name == "bwa"
                                BiFront<'M', bwa_wfa.p, 'F'> FrM_f;
                                BiFront<'I', bwa_wfa.p, 'F'> FrI_f;
                                BiFront<'D', bwa_wfa.p, 'F'> FrD_f;
                                BiFront<'M', bwa_wfa.p, 'R'> FrM_r;
                                BiFront<'I', bwa_wfa.p, 'R'> FrI_r;
                                BiFront<'D', bwa_wfa.p, 'R'> FrD_r;
                                FrontXM FrXM;
                                Front FrI, FrD;
                                aln_t = lbiwfa<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j, bwa_wfa.p>(q, t, c_f, c_r, FrM_f, FrI_f, FrD_f, FrM_r, FrI_r, FrD_r, FrXM, FrI, FrD, aln.data());
                                aln.resize(aln_t - aln.data());
                                s_t = eval_score<bwa_wfa.x, bwa_wfa.o, bwa_wfa.e, bwa_wfa.j>(aln, q, t);
                            }

                            auto end = std::chrono::high_resolution_clock::now();
                            std::chrono::duration<double, std::milli> elapsed = end - start;
                            total_time += elapsed.count();

                            if (s_t != *it)
                            {
                                std::cerr << "score not equal to python version\n"
                                          << t << '\n'
                                          << q << '\n';
                                std::exit(1);
                            }

                            ++it;
                        }

                        fin.close();
                    }
                }
            }
        }
    }
    std::cerr << total_time << " ms\n";
}
