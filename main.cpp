#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <ctime>
#include <stdlib.h>

using namespace std;

struct st_neuron
{
    st_neuron()
    {
        fired=false;
        trig_val=1;
    }
    st_neuron(int weight_count)
    {
        fired=false;
        for(int i=0;i<weight_count;i++)
        {
            vec_w.push_back( float((int)rand()%200)/100.0-1.0 );
        }
        trig_val=float((int)rand()%100)/100.0;
    }

    vector<float> vec_w;
    float trig_val;
    bool fired;
};

struct st_nn
{
    st_nn()
    {
        score=0;
        score_final=0;
    }
    st_nn(int size_x,int size_y)
    {
        score=0;
        score_final=0;
        kill=false;
        neuron_layers_x=size_x;
        neuron_layers_y=size_y;
        for(int x=0;x<neuron_layers_x;x++)
        {
            vv_neurons.push_back(vector<st_neuron>());
            for(int y=0;y<neuron_layers_y;y++)
            {
                vv_neurons[x].push_back(st_neuron(neuron_layers_y));
            }
        }
    }

    vector< vector<st_neuron> > vv_neurons;
    int   neuron_layers_x,neuron_layers_y;
    float score,score_final;
    bool  kill;
    int   info;

    float calc_score(int input_value)
    {
        //give input to first layer
        //cout<<input_value<<endl;
        //cout<<vv_neurons.size()<<endl;
        //cout<<vv_neurons[0].size()<<endl;
        for(int i=0;i<neuron_layers_y;i++)
        {
            if(i==input_value)  vv_neurons[0][i].fired=true;
            else                vv_neurons[0][i].fired=false;
        }

        //run sim
        for(int x=1;x<neuron_layers_x;x++)
        {
            for(int y=0;y<neuron_layers_y;y++)
            {
                //calc input signal
                float signal=0;
                for(int i=0;i<neuron_layers_y;i++)
                {
                    if(vv_neurons[x-1][i].fired)
                        signal+=vv_neurons[x][y].vec_w[i];
                }
                if(signal>vv_neurons[x][y].trig_val)    vv_neurons[x][y].fired=true;
                else                                    vv_neurons[x][y].fired=false;
            }
        }
        //get score
        score=0;
        for(int i=0;i<neuron_layers_y;i++)
        {
            if(vv_neurons[neuron_layers_x-1][i].fired)
            {
                if(i==input_value)  score+=100;
                else                score-=10;
            }
        }

        return score;
    }

    st_nn operator=(st_nn object_to_copy)
    {
        neuron_layers_x=object_to_copy.neuron_layers_x,
        neuron_layers_y=object_to_copy.neuron_layers_y;
        score=object_to_copy.score,
        score_final=object_to_copy.score_final;
        kill=object_to_copy.kill;

        vv_neurons.clear();
        for(int x=0;x<neuron_layers_x;x++)
        {
            vv_neurons.push_back(vector<st_neuron>());
            for(int y=0;y<neuron_layers_y;y++)
            {
                vv_neurons[x].push_back(object_to_copy.vv_neurons[x][y]);
            }
        }

        return *this;
    }

    st_nn mate_with(st_nn other_nn)
    {
        st_nn child;
        child=other_nn;//to be safe
        score=0;
        score_final=0;
        kill=false;

        //start mixing
        for(int x=0;x<neuron_layers_x;x++)
        {
            for(int y=0;y<neuron_layers_y;y++)
            {
                //pick parents trig value
                int rand_val=rand()%3;
                switch(rand_val)
                {
                    case 0: child.vv_neurons[x][y].trig_val=vv_neurons[x][y].trig_val; break;
                    case 1: child.vv_neurons[x][y].trig_val=other_nn.vv_neurons[x][y].trig_val; break;
                    case 2: child.vv_neurons[x][y].trig_val=0.5*(vv_neurons[x][y].trig_val+other_nn.vv_neurons[x][y].trig_val); break;
                }

                for(int n=0;n<(int)child.vv_neurons[x][y].vec_w.size();n++)
                {
                    //pick parents value
                    int rand_val=rand()%3;
                    switch(rand_val)
                    {
                        case 0: child.vv_neurons[x][y].vec_w[n]=vv_neurons[x][y].vec_w[n]; break;
                        case 1: child.vv_neurons[x][y].vec_w[n]=other_nn.vv_neurons[x][y].vec_w[n]; break;
                        case 2: child.vv_neurons[x][y].vec_w[n]=0.5*(vv_neurons[x][y].vec_w[n]+other_nn.vv_neurons[x][y].vec_w[n]); break;
                    }
                }
            }
        }

        return child;
    }

    void mutate_values(int chance, float sens=0.1)
    {
        for(int x=0;x<neuron_layers_x;x++)
        {
            for(int y=0;y<neuron_layers_y;y++)
            {
                if(rand()%chance==0)
                {
                    //change value by max 50% * sens
                    float rand_val=sens*(float((int)rand()%100)/100.0-0.5);
                    vv_neurons[x][y].trig_val+=vv_neurons[x][y].trig_val*rand_val;
                }

                for(int n=0;n<(int)vv_neurons[x][y].vec_w.size();n++)
                {
                    //flip coin
                    if(rand()%chance==0) continue;//skip

                    //change value by max 50% * sens
                    float rand_val=sens*(float((int)rand()%100)/100.0-0.5);
                    vv_neurons[x][y].vec_w[n]+=vv_neurons[x][y].vec_w[n]*rand_val;
                }
            }
        }
    }
};

int main()
{
    cout<<"Startup\n";

    srand(time(0));

    //init neuron grids
    cout<<"Network init\n";
    int neuron_layers_x=10;
    int neuron_layers_y=10;
    int pop_size=1000;
    vector<st_nn> v_networks;
    for(int i=0;i<pop_size;i++)
    {
        v_networks.push_back(st_nn(neuron_layers_x,neuron_layers_y));
    }

    //enter optimization loop
    int winner_nn_ind=-1;
    float target_score=100;
    while(true)
    {
        //calc score
        float best_score=0;
        int best_nn_ind=-1;
        float score_limit=0;
        //cout<<"Calc score\n";
        int runs_per_nn=10;
        for(int i=0;i<pop_size;i++)
        {
            float score_avg=0;
            for(int j=0;j<runs_per_nn;j++)
            {
                score_avg+=v_networks[i].calc_score( j );
            }
            score_avg/=(float)runs_per_nn;
            v_networks[i].score_final=score_avg;

            if(score_avg>best_score)
            {
                best_score=score_avg;
                best_nn_ind=i;

                //done test
                if(best_score>=target_score)
                {
                    winner_nn_ind=i;
                    break;
                }
            }
            else if(score_avg==best_score)//if same score, flip coin
            {
                if(rand()%2==0)
                {
                    best_score=score_avg;
                    best_nn_ind=i;
                }
            }

            score_limit+=score_avg;
        }
        if(winner_nn_ind!=-1) break;
        score_limit/=pop_size;

        //print score
        cout<<"Best score: "<<best_score<<"\tby: "<<best_nn_ind<<"\tfrom: "<<v_networks[best_nn_ind].info<<endl;

        //remove low score nn
        int counter=0;
        for(int i=0;i<pop_size;i++)
        {
            if(v_networks[i].score_final<score_limit)
            {
                v_networks[i].kill=true;
                counter++;
            }
        }
        //cout<<"Kill count: "<<counter<<endl;

        //fill pop by mixing
        for(int i=0;i<pop_size;i++)
        {
            if(v_networks[i].kill)
            {
                if(rand()%5==0)
                {
                    //new random values
                    for(int x=0;x<(int)v_networks[i].vv_neurons.size();x++)
                    {
                        for(int y=0;y<(int)v_networks[i].vv_neurons[x].size();y++)
                        {
                            v_networks[i].vv_neurons[x][y]=st_neuron(neuron_layers_y);
                        }
                    }
                    v_networks[i].info=-2;
                }
                else if(rand()%4==0)
                {
                    //mutate leader
                    v_networks[i]=v_networks[best_nn_ind];
                    v_networks[i].mutate_values(10);
                    v_networks[i].info=-3;
                }
                else //mate
                {
                    //find parents
                    int parent1_ind=best_nn_ind;
                    //int parent1_ind=rand()%pop_size;
                    int parent2_ind=rand()%pop_size;
                    for(int j=parent1_ind;j<pop_size;j++)
                    {
                        if(!v_networks[j].kill)
                        {
                            parent1_ind=j;
                            break;
                        }
                        //loop
                        if(j==pop_size-1) j=0;
                    }
                    for(int j=parent2_ind;j<pop_size;j++)
                    {
                        if(!v_networks[j].kill)
                        {
                            parent2_ind=j;
                            break;
                        }
                        //loop
                        if(j==pop_size-1) j=0;
                    }

                    v_networks[i]=v_networks[parent1_ind].mate_with(v_networks[parent2_ind]);
                    v_networks[i].info=parent1_ind;
                }
            }
        }

        //mutate parents
        for(int i=0;i<pop_size;i++)
        {
            if(!v_networks[i].kill)
            {
                /*if(v_networks[i].score_final!=best_score)//do not touch best set
                    v_networks[i].mutate_values();
                else cout<<"best found\n";*/

                if(i!=best_nn_ind)//do not touch best set
                {
                    if(best_score>95) v_networks[i].mutate_values(10,0.05);
                    else if(best_score>90) v_networks[i].mutate_values(10,0.1);
                    else v_networks[i].mutate_values(5,0.2);
                    v_networks[i].mutate_values(10);
                    v_networks[i].info=-1;
                }
            }
            else v_networks[i].kill=false;
        }
    }
    //optimization loop done

    cout<<"Winner found\n";
    ofstream output_nn("nn_data.txt");
    if(output_nn==0)
    {
        cout<<"ERROR: Could not make file\n";
        return 1;
    }
    //print header
    output_nn<<"Neural network values from NN: "<<winner_nn_ind<<endl;
    output_nn<<v_networks[winner_nn_ind].score_final<<"\tFinal score"<<endl;
    output_nn<<v_networks[winner_nn_ind].neuron_layers_x<<"\tLayer count x"<<endl;
    output_nn<<v_networks[winner_nn_ind].neuron_layers_y<<"\tLayer count y"<<endl;
    output_nn<<"Weights and trigger values\n";
    for(int x=0;x<(int)v_networks[winner_nn_ind].vv_neurons.size();x++)
    {
        for(int y=0;y<(int)v_networks[winner_nn_ind].vv_neurons[x].size();y++)
        {
            for(int n=0;n<(int)v_networks[winner_nn_ind].vv_neurons[x][y].vec_w.size();n++)
            {
                output_nn<<v_networks[winner_nn_ind].vv_neurons[x][y].vec_w[n]<<" ";
            }
            output_nn<<v_networks[winner_nn_ind].vv_neurons[x][y].trig_val<<endl;
        }
    }
    output_nn.close();

    cout<<"Complete\n\n";

    return 0;
}
