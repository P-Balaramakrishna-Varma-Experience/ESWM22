import './App.css';
import { Component, useState } from 'react';
import { Line } from 'react-chartjs-2';
import React from 'react';
import {
  Chart as ChartJS,
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend,
} from 'chart.js';

const axios = require('axios');

ChartJS.register(
  CategoryScale,
  LinearScale,
  PointElement,
  LineElement,
  Title,
  Tooltip,
  Legend
);

class Visual extends Component{
  constructor(props) {
    super()
    let data = {
      labels: [0],
      datasets: [
        {
          label: 'Dataset 1',
          data: [0],
          borderColor: 'rgb(255, 99, 132)',
          backgroundColor: 'rgba(255, 99, 132, 0.5)',
        },
      ],
    };
    let options = {
      responsive: true,
      plugins: {
        legend: {
          position: 'top',
        },
        title: {
          display: true,
          text: 'Chart.js Line Chart',
        },
      },
      maintainAspectRatio: false,
    };
    this.state = {options: options, data: data}
  }

  async componentDidMount() {
    try 
    {
      setInterval(async () => {
        try 
        {
          const res = await axios.get('http://localhost:4000/latest');
          const rpm_values = res.data.data;
          console.log(res.data);
          console.log(rpm_values)
          
          this.setState((prevState) => {
            let newState = JSON.parse(JSON.stringify(prevState));
            newState.data.labels = rpm_values.map((value, index) => index + 1);
            newState.data.datasets[0].data = rpm_values;
            return newState;
          })
        } 
        catch(err) 
        {
          console.log(err)
        }
      }, 3000);
    } 
    catch(e) 
    {
     console.log(e);
    }
  }

  render() { 
    return (
      <div>
        <Line options={this.state.options} data={this.state.data} height="600" />
      </div>
    )
  }
}



function App() {
  const [data, setData] = useState({k_p:0, k_d:0, k_i:0, run:false, desired: 0})
  const change_handler = e => {
    const {name, value}  = e.target
    
      setData({
          ...data,
          [name]: parseFloat(value)
      })
  }


  const start_motor = () => {
    setData({
      ...data,
      run: true
    })

    axios({
      method: 'post',
      url: 'http://localhost:4000/',
      data: data
    })
    .then((response) => {
      console.log("axios request sent fine")
    })
    .catch((error) => {
      console.log(error);
      console.log("not fine")
    });

    console.log(data)
  }

  
  const stop_motor = () => {
    setData({k_p:0, k_d:0, k_i:0, run:false, desired: 0})

    axios({
      method: 'post',
      url: 'http://localhost:4000/',
      data: data
    })
    .then((response) => {
      console.log("axios request to set kp kc sent fine")
    })
    .catch((error) => {
      console.log(error);
      console.log("not fine to set kp kc")
    });

    axios({
      method: 'get',
      url: 'http://localhost:4000/om2m/reset',
    })
    .then((response) => {
      console.log("axios request to reset data cont sent fine")
    })
    .catch((error) => {
      console.log(error);
      console.log("not fine request to reset data cont")
    });



    console.log(data)
  }

  
  

  return (
    <div className="App">
      {console.log(data)}
      <h1>PID Control Of DC Motor</h1>
      <form>
        <label><h3>K_p: <input type="number" name="k_p" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>K_i: <input type="number" name="k_i" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>K_d: <input type="number" name="k_d" onChange={change_handler} step="0.01"></input></h3></label>
        <label><h3>Desired Value: <input type="Number" name="desired" onChange={change_handler} step="0.01"></input></h3></label>
        <button type="button" onClick={start_motor}>Start</button>
        <button type="button" onClick={stop_motor}>Stop</button>
        <br></br>
      </form>
      <br></br>
      <br></br>
      {
        data.run == true && 
        <iframe width="400" height="300" src="http://192.168.43.168/"></iframe>
      }
      {
        data.run == true &&
        <Visual></Visual>
      }
    </div>
  );
}

export default App;